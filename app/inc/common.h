#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>

#define TEST_MODE_COUNT         5
#define CUSTOM_MSG_COUNT        5
#define CUSTOM_MSG_LENGTH       10

#ifdef DEBUG
#define PRINT_DEBUG(...) printf(__VA_ARGS__)
#else
#define PRINT_DEBUG(...)
#endif

enum mode {
        MODE_NORMAL = 0,
        MODE_CUSTOM_MSG = 1,
        MODE_CUSTOM_MSG_ERR = 2,
        MODE_STOP_SENDING = 3,
        MODE_EXIT = 4
};

extern pthread_mutex_t  program_mutex;
extern enum mode        program_mode;
extern char             *device_path;
extern uint8_t          msg_option; //1,2,3,4,5
extern uint8_t          error_index; //0-n
extern uint8_t          error_value; // ASCII char
extern uint8_t          led; // green, red
extern uint16_t         unit_duration; //ms
extern sem_t            semFinishSignal;
extern sem_t            semStart;

#endif
