#ifndef EXECUTE_REGIME_H
#define EXECUTE_REGIME_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>

typedef struct test_pairs{
        const char* test_input;
        const char* test_output;
}test_pairs_t;

enum mode {
        MODE_NORMAL = 0,
        MODE_CUSTOM_MSG = 1,
        MODE_CUSTOM_MSG_ERR = 2,
        MODE_STOP_SENDING = 3
};

extern pthread_mutex_t mutex;
extern enum mode       program_mode;
extern uint8_t         msg_option; //1,2,3,4,5
extern uint8_t         error_index; //0-n
extern uint8_t         error_value; // ASCII char
extern uint8_t         led; // green, red
extern uint16_t        unit_duration; //ms
extern sem_t semFinishSignal;
extern sem_t semStart;

static char* rand_string(void);

void test_setup(void);
uint8_t normal_regime(const char* );
uint8_t test_regime(const uint8_t, const char*);

#endif