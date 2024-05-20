#include "kernel/types.h"
#include "kernel/riscv.h"
// #include "kernel/stat.h"
#include "user/user.h"

#define ELEMENTS (7 * (PGSIZE / sizeof(int)))
#define ARRSIZE (ELEMENTS * sizeof(int))

int stack_arr[ELEMENTS];

void check(int* array, int size, int expected) {
  int accessed;
  printf("Test access %d: ", expected);
  if (pgaccess(array, size) != 0) {
    fprintf(2, "pgaccess non-zero exit code\n");
    exit(1);
  }
  if (expected != 0) {
    array[0] = 42;
  }
  if ((accessed = pgaccess(array, size)) != expected) {
    fprintf(2, "expected accessed = %d, got %d\n", expected, accessed);
    exit(2);
  }
  printf("success!\n");
}

int main() {
  int *heap_arr = malloc(ARRSIZE);
  
  printf("Running stack test...\n");
  pgaccess(stack_arr, ARRSIZE); // reset
  check(stack_arr, ARRSIZE, 0);
  check(stack_arr, ARRSIZE, 1);

  printf("Running heap test...\n");
  pgaccess(heap_arr, ARRSIZE); // reset
  check(heap_arr, ARRSIZE, 0);
  check(heap_arr, ARRSIZE, 1);

  free(heap_arr);
  return 0;
}
