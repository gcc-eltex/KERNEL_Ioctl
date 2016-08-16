#include "header/chdev.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivannikov Igor");
MODULE_DESCRIPTION("Testing char dev module");

static dev_t			dev;		// Мажорный и минорный номера
						// для создания устройства
static struct cdev		cdev;
static struct class		*class;
static struct device		*device;
static struct file_operations	fops = {
	.unlocked_ioctl = device_ioctl
};

int init_module(void)
{
	if (alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME) < 0)
		goto err_reg;
	cdev_init(&cdev, &fops);
	if (cdev_add(&cdev, dev, 1) < 0)
		goto err_add;

	class = class_create(THIS_MODULE, DEVICE_CLASS_NAME);
	if (IS_ERR(class))
		goto err_class;
	class->devnode = device_devnode;

	device = device_create(class, NULL, dev, NULL, DEVICE_NAME);
	if (IS_ERR(device))
		goto err_dev;

	printk(KERN_ALERT "Created a device with major number %d\n", MAJOR(dev));
	return 0;

err_reg:
	printk(KERN_ERR "Failed to get the major number at module init\n");
	return -1;

err_add:
	printk(KERN_ERR "Failed adding device when the module init\n");
	unregister_chrdev_region(dev, 1);
	return -1;

err_class:
	printk(KERN_ERR "Failed to create class at module init\n");
	cdev_del(&cdev);
	unregister_chrdev_region(dev, 1);
	return -1;

err_dev:
	printk(KERN_ERR "Failed to create device file at module init\n");
	class_destroy(class);
	cdev_del(&cdev);
	unregister_chrdev_region(dev, 1);
	return -1;
}

void cleanup_module(void)
{
	device_destroy(class, dev);
	class_destroy(class);
	cdev_del(&cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_ALERT "Removed device with major number %d\n", MAJOR(dev));
}

/*
 * device_ioctl - выполняет прием или передачу данных межуду user и kernel
 * space в соответствии с командой переданной при вызове ioctl в user space.
 *
 * Возвращает количество считанных байт при успешном завершении, а при
 * некорректной коменде -ENOTTY.
 */
static long device_ioctl(struct file *filp, unsigned int cmd,
			unsigned long arg)
{
	char send_msg[] = "Hello, user!\0";
	char recv_msg[64];
	long nbyte;
	char *udata = (char __user *)arg;

	switch (cmd) {
		case CHDEV_IOCTLREAD:
			nbyte = 0;
			while (send_msg[nbyte] != '\0') {
				udata[nbyte] = send_msg[nbyte];
				nbyte++;
			}
			printk(KERN_ALERT "User read from ioctl\n");
			return nbyte - 1;

		case CHDEV_IOCTLWRITE:
			nbyte = 0;
			while (udata[nbyte] != '\0') {
				recv_msg[nbyte] = udata[nbyte];
				nbyte++;
			}
			recv_msg[nbyte] = '\0';
			printk(KERN_ALERT "User write from ioctl: %s\n", recv_msg);
			return 0;

		default:
			return -ENOTTY;
	}
}

/*
 * Какая-то дичь! Нужна, чтобы выставить права на созданный файл символьного
 * устройства.
 */
static char *device_devnode(struct device *dev, umode_t *mode)
{
		if (mode)
			*mode = 0666;
		return NULL;
}