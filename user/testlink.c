#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/user.h"

#define TESTS_COUNT 12
#define DATA_SIZE 13

char name_buf[MAXPATH], data_buf[DATA_SIZE];
const char data[DATA_SIZE] = "Oh hi, Mark!\0";

int check(int result, int expected, const char* message) {
  if (expected != -1) {
    if (result != expected) {
      fprintf(2, "%s! Expected %d, got %d.\n", message, expected, result);
      return -1;
    }
  } else if (result < 0) {
    fprintf(2, "%s! Exit code %d.\n", message, result);
    return -1;
  }
  return 1;
}

void makefile(char *filename) {
  int fd = open(filename, O_CREATE | O_WRONLY);
  check(fd, -1, "Could not create file");
  check(write(fd, data, DATA_SIZE), DATA_SIZE, "Could not write");
  close(fd);
}

void read_data(const char *filename, char *buf) {
  int fd = open(filename, O_RDONLY);
  check(fd, -1, "Could not open file");
  check(read(fd, buf, DATA_SIZE), DATA_SIZE, "Could not read");
  buf[DATA_SIZE - 1] = 0;
}

int test1() {
  int pass = -1;
  chdir("/");
  symlink("f0", "s0_abs");
  pass += check(readlink("s0_abs", name_buf), -1, "Readlink error");

  read_data(name_buf, data_buf);

  pass += check(strcmp(data_buf, data), 0, "Contents do not match");
  return pass;
}

int test2() {
  int pass = -1;
  return pass;
}

int test3() {
  int pass = -1;
  return pass;
}

int test4() {
  int pass = -1;
  return pass;
}

int test5() {
  int pass = -1;
  return pass;
}

int test6() {
  int pass = -1;
  return pass;
}

int test7() {
  int pass = -1;
  return pass;
}

int test8() {
  int pass = -1;
  return pass;
}

int test9() {
  int pass = -1;
  return pass;
}

int test10() {
  int pass = -1;
  return pass;
}

int test11() {
  int pass = -1;
  return pass;
}

int test12() {
  int pass = -1;
  return pass;
}

static int (*tests[TESTS_COUNT])(void) = {
    test1,
    test2,
    test3,
    test4,
    test5,
    test6,
    test7,
    test8,
    test9,
    test10,
    test11,
    test12,
};

static char* test_names[TESTS_COUNT] = {
    "correct absolute",
    "correct relative",
    "correct relative up",
    "correct relative down",
    "correct absolute to absolute",
    "correct absolute to relative",
    "correct relative to relative",
    "endless rec",
    "endless rec relative",
    "absolute to nonexistent",
    "relative to wrong path up",
    "relative to wrong path down",
};

void run_test(uint index) {
  printf("Running test `%s`... ", test_names[index]);
  if (check(tests[index](), -1, "Fail") >= 0) {
    printf("Success!\n");
  }
}

void make_files() {
  mkdir("l1");
  mkdir("l1/l2");
  mkdir("l1/l2/l3");
  makefile("f0");
  makefile("l1/l2/l3/f3");
}

int main() {
  make_files();
  for (uint i = 0; i < TESTS_COUNT; ++i) {
    run_test(i);
  }
  return 0;
}