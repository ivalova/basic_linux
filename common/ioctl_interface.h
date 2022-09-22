#ifndef IOCTL_INTERFACE_H
#define IOCTL_INTERFACE_H

#define WR_VALUE _IOW('a','a',test_error_mode_data_t*)
#define RD_VALUE _IOR('a','b',test_error_mode_data_t*)

enum mode {
        MODE_NORMAL = 0,
        MODE_CUSTOM_MSG = 1,
        MODE_CUSTOM_MSG_ERR = 2,
        MODE_STOP_SENDING = 3,
        MODE_EXIT = 4
};

typedef struct test_error_mode_data {
        enum mode mode_regime;
        int char_index_to_change;
        char new_char_value;
} test_error_mode_data_t;

#endif