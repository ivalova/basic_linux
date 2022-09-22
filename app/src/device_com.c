#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "device_com.h"
#include "ioctl_interface.h"

static char* device_path = "/dev/morse";

static int ioctl_send(enum mode new_mode, uint8_t char_index_to_change, char new_char_value)
{
    int fd = open(device_path, O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "Error: cannot open char device\n");
        return -1;
    }

    const test_error_mode_data_t test_error_mode = {
            .mode_regime = new_mode,
            .char_index_to_change = char_index_to_change,
            .new_char_value = new_char_value
    };

    int ioctl_ret = ioctl(fd, WR_VALUE, (test_error_mode_data_t*) &test_error_mode);
    if (ioctl_ret)
    {
        fprintf(stderr, "Error: cannot perform ioctl to char device\n");
        return ioctl_ret;
    }

    return 0;
}

int ioctl_send_test_error(uint8_t char_index_to_change, char new_char_value)
{
    return ioctl_send(MODE_CUSTOM_MSG_ERR, char_index_to_change, new_char_value);
}

int ioctl_set_mode_to_normal()
{
    return ioctl_send(MODE_NORMAL, 0, 0);
}