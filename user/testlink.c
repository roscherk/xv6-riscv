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
  if (result < expected) {
    fprintf(2, "%s! Expected %d, got %d.\n", message, expected, result);
    return -1;
  }
  return 1;
}

void makefile(char *filename) {
  int fd = open(filename, O_CREATE | O_WRONLY);
  check(fd, 0, "Could not create file");
  check(write(fd, data, DATA_SIZE), DATA_SIZE, "Could not write");
  close(fd);
}

void read_data(const char *filename, char *buf) {
  int fd = open(filename, O_RDONLY);
  check(fd, 0, "Could not open file");
  check(read(fd, buf, DATA_SIZE), DATA_SIZE, "Could not read");
  buf[DATA_SIZE - 1] = 0;
  printf("read_data: buf = `%s`\n", buf);
}

void arrange(const char* dir) {
  memset(name_buf, 0, MAXPATH);
  memset(data_buf, 0, DATA_SIZE);
  chdir(dir);
}

int test1() {
  int pass = -1;
  arrange("/");

  symlink("f0", "l0_abs");
  pass += check(readlink("l0_abs", name_buf), -1, "Readlink error");

  read_data("l0_abs", data_buf);
  pass += check(strcmp(data_buf, data), 0, "Contents do not match");

  return pass;
}

int test2() {
  int pass = -1;
  arrange("/");

  symlink("./f0", "l0_rel");
  pass += check(readlink("l0_rel", name_buf), 0, "Readlink error");

  read_data("l0_rel", data_buf);
  pass += check(strcmp(data_buf, data), 0, "Contents do not match");

  return pass;
}

int test3() {
  int pass = -1;
  arrange("/d1/d2/d3");

  symlink("../../../f0", "l0_rel_up");
  pass += check(readlink("l0_rel_up", name_buf), -1, "Readlink error");

  read_data("l0_rel_up", data_buf);
  pass += check(strcmp(data_buf, data), 0, "Contents do not match");

  return pass;
}

int test4() {
  int pass = -1;
  arrange("/");

  symlink("./d1/d2/d3/f3", "l3_rel_down");
  pass += check(readlink("l3_rel_down", name_buf), -1, "Readlink error");

  read_data("l3_rel_down", data_buf);
  pass += check(strcmp(data_buf, data), 0, "Contents do not match");

  return pass;
}

int test5() {
  int pass = -1;
  arrange("/");

  symlink("l0_abs", "l0_abs2abs");
  pass += check(readlink("l0_abs2abs", name_buf), -1, "Readlink error");

  read_data("l0_abs2abs", data_buf);
  pass += check(strcmp(data_buf, data), 0, "Contents do not match");

  return pass;
}

int test6() {
  int pass = -1;
  arrange("/");

  symlink("l0_rel", "l0_abs2rel");
  pass += check(readlink("l0_abs2rel", name_buf), -1, "Readlink error");

  read_data("l0_abs2rel", data_buf);
  pass += check(strcmp(data_buf, data), 0, "Contents do not match");

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

//static char* link_names[TESTS_COUNT] = {
//    "l0_abs",
//    "l0_rel",
//    "l0_rel_up",
//    "l3_rel_down",
//    "l0_abs2abs",
//    "l0_abs2rel",
//    "l0_rel2rel",
//    "l_loop",
//    "l_loop_rel",
//    "l_abs2none",
//    "l3_rel2none_up",
//    "l0_rel2none_down",
//};

void run_test(uint index) {
  printf("Running test `%s`... ", test_names[index]);
  if (check(tests[index](), 0, "Fail") >= 0) {
    printf("Success!\n");
  }
}

void make_files() {
  mkdir("d1");
  mkdir("d1/d2");
  mkdir("d1/d2/d3");
  makefile("f0");
  makefile("d1/d2/d3/f3");
}

int main() {
  make_files();
  for (uint i = 0; i < TESTS_COUNT; ++i) {
    run_test(i);
  }
  return 0;
}