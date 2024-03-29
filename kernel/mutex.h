#include "sleeplock.h"

typedef struct mutex {
    struct sleeplock lock;
    unsigned dcount;
} mutex_t;
