#include "param.h"
#include "types.h"
#include "riscv.h" // каким-то образом правит кучу ошибок компиляции
#include "defs.h"
#include "spinlock.h"
#include "mutex.h"

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
        mutex_t mutex = mtable[i];
        acquiresleep(&mutex.lock);
        if (mutex.dcount == 0) {
            mutex.dcount++;
            releasesleep(&mutex.lock);
            return i;
        }
        releasesleep(&mutex.lock);
    }
    release(&mtable_lock);
    return -1;  // не нашли ни одного свободного дескриптора
}

int check_md(int md) {
    return md < 0 || md >= NMUTEX || mtable[md].dcount == 0 ? -1 : 0;
}

int mutex_dup(int md) {
    if (check_md(md) < 0)
        return -1;
    mtable[md].dcount++;
    return md;
}

int mutex_lock(int md) {
    if (check_md(md) < 0)
        return -1;
    mutex_t mutex = mtable[md];
    if (!holdingsleep(&mutex.lock))
        acquiresleep(&mutex.lock);
    return 0;
}

int mutex_unlock(int md) {
    if (check_md(md) < 0)
        return -1;
    mutex_t mutex = mtable[md];
    if (holdingsleep(&mutex.lock))
        releasesleep(&mutex.lock);
    return 0;
}

int mutex_release(int md) {
    if (check_md(md) < 0)
        return -1;
    mutex_t mutex = mtable[md];
    if (holdingsleep(&mutex.lock))
        releasesleep(&mutex.lock);
    mutex.dcount--;
    return 0;
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
