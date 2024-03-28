#include "sleeplock.h"

typedef struct {
    struct sleeplock lock;
    unsigned dcount;
} mutex_t;
