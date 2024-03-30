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
  for (int i = 0; i < NMUTEX; ++i) {
    initsleeplock(&mtable[i].lock, "mutex");
    mtable[i].dcount = 0;
  }
  release(&mtable_lock);
}

int mutex_create(void) {
  int md = -1;
  acquire(&mtable_lock);
  for (int i = 0; i < NMUTEX; ++i) {
    mutex_t *mutex = &mtable[i];
    acquiresleep(&mutex->lock);
    if (mutex->dcount == 0) {
      mutex->dcount++;

      struct proc *process = myproc();
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
    }
    releasesleep(&mutex->lock);
    printf("mutex->lock.locked = %d", mutex->lock.locked);
    if (md != -1) {
      printf("process->omutex[j]->lock.locked = %d", myproc()->omutex[md]->lock.locked);
      break;
    }
  }
  release(&mtable_lock);
  return md;
}

int check_md(int md) {
  printf("checking %d...\n", md);
  return md < 0 || md >= NOMUTEX ? -1 : md;
}

int mutex_lock(int md) {
  printf("mutex lock %d\n", md);
  for (int i = 0; i < NOMUTEX; ++i) {
    printf("myproc()->omutex[%d] = %d\n", i, myproc()->omutex[i]);
  }
  if (check_md(md) < 0)
    return -1;
  printf("myproc()->omutex[md]->lock.locked = %d", myproc()->omutex[md]->lock.locked);
  acquiresleep(&myproc()->omutex[md]->lock);
  printf("I AM ALIVE!!!");
  return 0;
}

int mutex_unlock(int md) {
  printf("mutex unlock %d\n", md);
  if (check_md(md) < 0)
    return -1;
  releasesleep(&myproc()->omutex[md]->lock);
  return 0;
}

int mutex_release(int md) {
  printf("mutex release %d\n", md);
  if (check_md(md) < 0)
    return -1;
  mutex_t *mutex = myproc()->omutex[md];
  releasesleep(&mutex->lock);
  mutex->dcount--;
  return 0;
}

mutex_t *mutex_dup(mutex_t *mutex) {
  mutex->dcount++;
  printf("mutex.dcount = %d now!\n", mutex->dcount);
  return mutex;
}

void mutex_close(mutex_t *mutex) { mutex->dcount--; }

int sys_mutex_create(void) { return mutex_create(); }

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
