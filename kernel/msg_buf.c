#include "param.h"
#include "types.h"
#include "riscv.h"
#include "spinlock.h"
#include "msg_buf.h"

// блокировать только на pr_message, на отдельные байты не надо

void msg_buf_init(void) {

}

void pr_mgs(const char* fmt, ...) {

}

uint64 sys_dmesg(void) {
  return 0;
}
