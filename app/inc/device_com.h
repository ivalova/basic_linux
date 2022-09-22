#ifndef DEVICE_COM_H
#define DEVICE_COM_H

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

void ioctl_write();

#endif