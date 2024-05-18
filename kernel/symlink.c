#include "types.h"
#include "riscv.h"
#include "defs.h"

int symlink(const char *target, const char *filename) {
  return 1;
}

int readlink(const char *filename, char *buf) {
  return 1;
}