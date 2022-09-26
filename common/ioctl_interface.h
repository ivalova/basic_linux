#ifndef IOCTL_INTERFACE_H
#define IOCTL_INTERFACE_H

#define WR_VALUE _IOW('a','a',enum mode*)

enum mode {
        MODE_NORMAL = 0,
        MODE_CUSTOM_MSG = 1,
        MODE_CUSTOM_MSG_ERR = 2,
        MODE_STOP_SENDING = 3,
        MODE_EXIT = 4
};

#endif