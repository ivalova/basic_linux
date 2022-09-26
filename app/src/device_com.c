#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "device_com.h"
#include "ioctl_interface.h"

static int ioctl_send_mode(enum mode new_mode, const char* device_path)
{
    int fd = open(device_path, O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "Error: cannot open char device\n");
        return -1;
    }

    int ioctl_ret = ioctl(fd, WR_VALUE, (enum mode*) &new_mode);
    if (ioctl_ret)
    {
        fprintf(stderr, "Error: cannot perform ioctl to char device\n");
        return ioctl_ret;
    }
    
    if (close(fd))
    {
        fprintf(stderr, "Error: cannot close char device\n");
        return -1;
    }

    return 0;
}

int ioctl_set_mode_to_test_error(const char* device_path)
{
    return ioctl_send_mode(MODE_CUSTOM_MSG_ERR, device_path);
}

int ioctl_set_mode_to_normal(const char* device_path)
{
    return ioctl_send_mode(MODE_NORMAL, device_path);
}

int ioctl_set_mode_to_test(const char* device_path)
{
    return ioctl_send_mode(MODE_CUSTOM_MSG, device_path);
}