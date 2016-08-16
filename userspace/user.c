#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define CHDEV_IOCTLREAD  0x80088101
#define CHDEV_IOCTLWRITE 0x40088102

int main()
{
    int  fd;
    char recv_msg[64];
    char send_msg[] = "Hello, kernel!\0";

    fd = open("/dev/my_chdev", O_RDWR);
    if (fd < 0){
        perror("open /dev/my_chdev");
        return -1;
    }

    ioctl(fd, CHDEV_IOCTLWRITE, send_msg);
    ioctl(fd, CHDEV_IOCTLREAD, recv_msg);
    printf("Kernel: %s\n", recv_msg);

    close(fd);
    return 0;
}