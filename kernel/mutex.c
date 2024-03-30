#include "param.h"
#include "types.h"
#include "riscv.h" // каким-то образом правит кучу ошибок компиляции
#include "defs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "mutex.h"
#include "proc.h"

mutex_t mtable[NMUTEX];
struct spinlock mtable_lock;

void mtable_init(void) {
    initlock(&mtable_lock, "mtable_lock");
    acquire(&mtable_lock);
    char name[NMUTEX][2];
    for (int i = 0; i < NMUTEX; ++i) {
        name[i][0] = (char)i + 97;
        name[i][1] = 0;
        printf("%d ", (char)i);
        initsleeplock(&mtable[i].lock, name[i]);
        mtable[i].dcount = 0;
    }
    for (int i = 0; i < NMUTEX; ++i) {
      printf("lock name = `");
      printf(mtable[i].lock.name);
      printf("`\n");
    }
    release(&mtable_lock);
}

int mutex_create(void) {
    int md = -1;
    acquire(&mtable_lock);
    for (int i = 0; i < NMUTEX; ++i) {
        mutex_t* mutex = &mtable[i];
        acquiresleep(&mutex->lock);
//        printf("I AM ALIVE!!! %d\n", i);
        if (mutex->dcount == 0) {
            mutex->dcount++;

            struct proc* process = myproc();
            printf("looking for a place...\n");
            for (int j = 0; j < NOMUTEX; ++j) {
                printf("p->omutex[%d] = %d\n", j, process->omutex[j]);
                if (process->omutex[j] == 0) {
                    printf("found place %d, setting it to %d\n", j, &mtable[i]);
                    process->omutex[j] = &mtable[i];
                    md = j;
                    break;
                }
            }

            releasesleep(&mutex->lock);
            return md;
        }
        releasesleep(&mutex->lock);
    }
    release(&mtable_lock);
    return -1;  // не нашли ни одного свободного дескриптора
}

int check_md(int md) {
    printf("checking %d...\n", md);
    return md < 0 || md >= NOMUTEX || mtable[md].dcount == 0 ? -1 : md;
}

int mutex_lock(int md) {
    printf("mutex lock `");
    printf(myproc()->omutex[md]->lock.name);
    printf("`\n");
    for (int i = 0; i < NOMUTEX; ++i) {
      printf("myproc()->omutex[%d] = %d, name = `", i, myproc()->omutex[i]);
      if (myproc()->omutex[i] != 0) {
        printf(myproc()->omutex[i]->lock.name);
      }
      printf("`\n");
    }
    if (check_md(md) < 0)
        return -1;
    printf("md = %d, lock = %d\n", md, myproc()->omutex[md]->lock.locked);
//    acquiresleep(&myproc()->omutex[md]->lock);
    printf("I AM ALIVE\n");
    return 0;
}

int mutex_unlock(int md) {
    printf("mutex unlock\n");
    if (check_md(md) < 0)
        return -1;
    releasesleep(&myproc()->omutex[md]->lock);
    return 0;
}

int mutex_release(int md) {
    printf("mutex release\n");
    if (check_md(md) < 0)
        return -1;
    mutex_t* mutex = myproc()->omutex[md];
    releasesleep(&mutex->lock);
    mutex->dcount--;
    return 0;
}

mutex_t* mutex_dup(mutex_t* mutex) {
    mutex->dcount++;
    printf("mutex.dcount = %d now!\n", mutex->dcount);
    return mutex;
}

void mutex_close(mutex_t* mutex) {
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
