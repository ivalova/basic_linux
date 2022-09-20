#ifndef EXECUTE_REGIME_H
#define EXECUTE_REGIME_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

static char *rand_string(void);

int normal_regime(void);

#endif