#ifndef DEVICE_COM_H
#define DEVICE_COM_H

#include <stdint.h>

#include "ioctl_interface.h"

int ioctl_set_mode_to_test_error(const char* device_path);
int ioctl_set_mode_to_normal(const char* device_path);
int ioctl_set_mode_to_test(const char* device_path);

#endif