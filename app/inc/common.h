#ifndef COMMON_H
#define COMMON_H

#define TEST_MODE_COUNT         5
#define CUSTOM_MSG_COUNT        5
#define CUSTOM_MSG_LENGTH       10

#ifdef DEBUG
#define PRINT_DEBUG(...) printf(__VA_ARGS__)
#else
#define PRINT_DEBUG(...)
#endif

#endif