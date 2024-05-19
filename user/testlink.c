#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void check(int result, const char* message) {
  if (result < 0) {
    fprintf(2, message);
    exit(1);
  }
}

void makefile(char* filename) {
  int fd = open(filename, O_CREATE);
  check(fd, "Could not create file");
  close(fd);
}

int main() {
  makefile("myfile");
  symlink("myfile", "symlink");
//  open("symlink", O_WRONLY);
  return 0;
}