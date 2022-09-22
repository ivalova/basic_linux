#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "device_com.h"

int32_t number = 13;
static char* device_path = "/dev/morse";

void ioctl_write()
{
    int fd = open(device_path, O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "Error: cannot open char device\n");
    }
    if (ioctl(fd, WR_VALUE, (int32_t*) &number))
    {
        fprintf(stderr, "Error: cannot perform ioctl to char device\n");
    }
}