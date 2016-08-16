#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "my_chdev"
#define DEVICE_CLASS_NAME "my_class_chdev"

/* Константы для ioctl */
#define IOCTL_MAGIC 0x81
#define CHDEV_IOCTLREAD  _IOR(IOCTL_MAGIC, 1, char *)
#define CHDEV_IOCTLWRITE _IOW(IOCTL_MAGIC, 2, char *)

int init_module(void);
void cleanup_module(void);
static char *device_devnode(struct device *, umode_t *);
static long device_ioctl(struct file *, unsigned int, unsigned long);