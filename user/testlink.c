#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/user.h"

#define TESTS_COUNT 13
#define DATA_SIZE 13

char name_buf[MAXPATH], data_buf[DATA_SIZE];
const char data[DATA_SIZE] = "Oh hi, Mark!\0";

void check(int result, int expected, const char *message) {
  if ((expected >= 0 && result < expected) ||
      (expected < 0 && result > expected)) {
    fprintf(2, "%s! Expected %d, got %d.\n", message, expected, result);
    exit(1);
  }
}

void makefile(char *filename) {
  int fd = open(filename, O_CREATE | O_WRONLY);
  check(fd, 0, "Could not create file");
  check(write(fd, data, DATA_SIZE), DATA_SIZE, "Could not write");
  close(fd);
}

void read_data(const char *filename, int expected) {
  int fd = open(filename, O_RDONLY);
  check(fd, expected, "Could not open file");
  check(read(fd, data_buf, DATA_SIZE), expected, "Could not read");
  data_buf[DATA_SIZE - 1] = 0;
}

void arrange(const char *dir) {
  memset(name_buf, 0, MAXPATH);
  memset(data_buf, 0, DATA_SIZE);
  chdir(dir);
}

void test1() {
  arrange("/");

  symlink("/f0", "l0_abs");
  check(readlink("l0_abs", name_buf), 0, "Readlink error");

  read_data("l0_abs", 0);
  check(strcmp(data_buf, data), 0, "Contents do not match");
}

void test2() {
  arrange("/");

  symlink("./f0", "l0_rel");
  check(readlink("l0_rel", name_buf), 0, "Readlink error");

  read_data("l0_rel", 0);
  check(strcmp(data_buf, data), 0, "Contents do not match");
}

void test3() {
  arrange("/d1/d2/d3");

  symlink("../../../f0", "l0_rel_up");
  read_data("l0_rel_up", 0);
  check(strcmp(data_buf, data), 0, "Contents do not match");
}

void test4() {
  arrange("/");

  symlink("./d1/d2/d3/f3", "l3_rel_down");
  read_data("l3_rel_down", 0);
  check(strcmp(data_buf, data), 0, "Contents do not match");
}

void test5() {
  arrange("/d1");

  symlink("/l0_abs", "l0_abs2abs");
  read_data("l0_abs2abs", 0);
  check(strcmp(data_buf, data), 0, "Contents do not match");
}

void test6() {
  arrange("/");

  symlink("/l0_rel", "l0_abs2rel");
  read_data("l0_abs2rel", 0);
  check(strcmp(data_buf, data), 0, "Contents do not match");
}

void test7() {
  arrange("/d1");

  symlink("../l0_rel", "l0_rel2rel");
  read_data("l0_rel2rel", 0);
  check(strcmp(data_buf, data), 0, "Contents do not match");
}

void test8() {
  arrange("/");

  symlink("l_inf_rec", "l_inf_rec");
  return read_data("l_inf_rec", -1);
}

void test9() {
  arrange("/");

  symlink("l0_rec", "l1_rec");
  symlink("l1_rec", "l2_rec");
  symlink("l2_rec", "l0_rec");
  return read_data("l0_rec", -1);
}

void test10() {
    arrange("/");

    symlink("no_file", "l_nofile_abs");
    return read_data("l_nofile_abs", -1);
}

void test11() {
  arrange("/");

  symlink("./f3", "l_wrong_rel");
  return read_data("l_wrong_rel", -1);
}

void test12() {
  arrange("/d1/d2/d3");

  symlink("../../../f3", "l_wrel_up");
  return read_data("l_wrel_up", -1);
}

void test13() {
  arrange("/");

  symlink("./d1/d2/d3/f0", "l_wrel_down");
  return read_data("l_wrel_down", -1);
}

static void (*tests[TESTS_COUNT])(void) = {
    test1, test2, test3, test4,  test5,  test6,
    test7, test8, test9, test10, test11, test12,
    test13
};

static char *test_names[TESTS_COUNT] = {
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
    "relative to wrong path",
    "relative to wrong path up",
    "relative to wrong path down",
};

void run_test(uint index) {
  printf("--- Running test %d `%s`... ", index + 1, test_names[index]);
  tests[index]();
  printf("Success!\n");
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