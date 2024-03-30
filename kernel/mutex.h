typedef struct mutex {
    struct sleeplock lock;
    unsigned dcount;
} mutex_t;
