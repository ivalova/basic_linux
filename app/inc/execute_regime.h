#ifndef EXECUTE_REGIME_H
#define EXECUTE_REGIME_H

#include "common.h"

struct test_pairs{
        const char* test_input;
        const char* test_output;
};

static char* rand_string(void);

void test_setup(void);
void normal_regime(void);
void test_regime(void);
void* execute_regime(void* );

#endif
