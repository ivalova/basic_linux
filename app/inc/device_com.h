#ifndef DEVICE_COM_H
#define DEVICE_COM_H

#include <stdint.h>

#include "ioctl_interface.h"

void ioctl_send_test_error(uint8_t char_index_to_change, char new_char_value);

#endif