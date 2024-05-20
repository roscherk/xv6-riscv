#include "kernel/types.h"
#include "kernel/riscv.h"
// #include "kernel/stat.h"
#include "user/user.h"

#define ARRSIZE (7 * (PGSIZE / 8))

int stack_arr[ARRSIZE];

void check(int* array, int size, int expected) {
  int accessed = 0;
  if (expected != 0) {
    array[0] = 42;
  }
  printf("Test access %d: ", expected);
  if (pgaccess(array, size, &accessed) != 0) {
    fprintf(2, "pgaccess non-zero exit code\n");
    exit(1);
  }
  if (accessed != expected) {
    fprintf(2, "expected accessed = %d, got %d\n", expected, accessed);
    exit(2);
  }
  printf("success!\n");
}

int main() {
  int *heap_arr = malloc(ARRSIZE);
  
  printf("Running stack test...\n");
  check(stack_arr, ARRSIZE, 0);
  check(stack_arr, ARRSIZE, 1);

  printf("Running heap test...\n");
  check(heap_arr, ARRSIZE, 0);
  check(heap_arr, ARRSIZE, 1);

  free(heap_arr);
  return 0;
}
