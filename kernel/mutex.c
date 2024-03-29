#include "param.h"
#include "types.h"
#include "riscv.h" // каким-то образом правит кучу ошибок компиляции
#include "defs.h"
#include "spinlock.h"
#include "mutex.h"
#include "proc.h"

mutex_t mtable[NMUTEX];
struct spinlock mtable_lock;

void mtable_init(void) {
    initlock(&mtable_lock, "mtable_lock");
    acquire(&mtable_lock);
    for (int i = 0; i < NMUTEX; ++i) {
        initsleeplock(&mtable[i].lock, "mutex_lock");
        mtable[i].dcount = 0;
    }
    release(&mtable_lock);
}

int mutex_create(void) {
    acquire(&mtable_lock);
    for (int i = 0; i < NMUTEX; ++i) {
        mutex_t* mutex = &mtable[i];
        acquiresleep(&mutex->lock);
//        printf("I AM ALIVE!!! %d\n", i);
        if (mutex->dcount == 0) {
            mutex->dcount++;

            struct proc* process = myproc();
            acquire(&process->lock);
            printf("looking for a place...\n");
            for (int j = 0; j < NOMUTEX; ++j) {
                // test_mutex test

                printf("p->omutex[%d] = %d\n", j, process->omutex[j]);
                if (process->omutex[j] == 0) {
                    printf("found place %d, setting it to %d\n", j, &mtable[i]);
                    process->omutex[j] = &mtable[i];
                    break;
                }
            }
            release(&process->lock);

            releasesleep(&mutex->lock);
            printf("mutex->dcount = %d, mtable[i].dcount = %d\n", mutex->dcount, mtable[i].dcount);
//            printf("found %d\n", i);
            return i;
        }
        releasesleep(&mutex->lock);
    }
    release(&mtable_lock);
    return -1;  // не нашли ни одного свободного дескриптора
}

int check_md(int md) {
    printf("checking %d...\n", md);
    return md < 0 || md >= NMUTEX || mtable[md].dcount == 0 ? -1 : md;
}

int mutex_lock(int md) {
    printf("mutex lock\n");
    if (check_md(md) < 0)
        return -1;
    printf("md = %d, lock = %d\n", md, mtable[md].lock.locked);
    acquiresleep(&mtable[md].lock);
    printf("I AM ALIVE\n");
    return 0;
}

int mutex_unlock(int md) {
    printf("mutex unlock\n");
    md = check_md(md);
    if (md < 0)
        return -1;
    releasesleep(&mtable[md].lock);
    return 0;
}

int mutex_release(int md) {
    printf("mutex release\n");
    md = check_md(md);
    if (md < 0)
        return -1;
    mutex_t* mutex = &mtable[md];
    releasesleep(&mutex->lock);
    mutex->dcount--;
    return 0;
}

mutex_t* mutex_dup(mutex_t* mutex) {
    mutex->dcount++;
    printf("mutex.dcount = %d now!\n", mutex->dcount);
    return mutex;
}

void mutex_rem(mutex_t* mutex) {
    mutex->dcount--;
}

int sys_mutex_create(void) {
    return mutex_create();
}

int sys_mutex_lock(void) {
    int md;
    argint(0, &md);
    return mutex_lock(md);
}

int sys_mutex_unlock(void) {
    int md;
    argint(0, &md);
    return mutex_unlock(md);
}

int sys_mutex_release(void) {
    int md;
    argint(0, &md);
    return mutex_release(md);
}
