#ifndef XV6_RISCV_PROCINFO_H
#define XV6_RISCV_PROCINFO_H

// костыль, но подключение procstate из proc.h всё ломает :(
enum my_procstate { MY_UNUSED, MY_USED, MY_SLEEPING, MY_RUNNABLE, MY_RUNNING, MY_ZOMBIE };

struct procinfo {
    char name[16];
    enum my_procstate state;
    int parent_id;
    int pid;
};

#endif //XV6_RISCV_PROCINFO_H
