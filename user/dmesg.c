#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/riscv.h"
#include "kernel/spinlock.h"
#include "kernel/msg_buf.h"
#include "user.h"

int main() {
  char *buf = (char*)malloc(BUFSIZE);
  if (dmesg(buf) < 0) {
    printf("Error: dmesg non-zero exit code\n");
    exit(-1);
  }
  printf("%s", buf);
  return 0;
}
