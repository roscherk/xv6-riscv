#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "user/user.h"

#define MOD 1000000007LU
#define FILENAME "large_file"
#define BUFSIZE 256  // uint64
#define BUFSIZE_B (BUFSIZE * sizeof(uint64)) // in bytes

uint64 next(uint64 current) { return (current * 239 + 42) % MOD; }

void generate_file(uint size, uint first) {
  int fd = open(FILENAME, O_CREATE | O_WRONLY);
  if (fd < 0) {
    fprintf(1, "Error: could not open file on write\n");
    exit(1);
  }

  uint64 current = (uint64)first;
 printf("current = %d\n", current);
  uint64 buf[BUFSIZE];

  int status;
  for (uint i = 0; i < size; ++i) {
    buf[i % BUFSIZE] = current;
    current = next(current);
    printf("buffer:");
    for (uint j = 0; j < BUFSIZE; ++j) {
      printf(" %d", buf[j]);
    }
    printf("\n");

    if (i > 0 && i % (BUFSIZE - 1) == 0) {
      status = write(fd, buf, BUFSIZE_B);
      printf("status = %d\n", status);
      if (status != BUFSIZE_B) {
        fprintf(1, "Error: could not write to file\n");
        close(fd);
        exit(2);
      }
    }
  }

  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(1, "Usage: test_lf {size} {first}\n");
    exit(1);
  }

  uint size = atoi(argv[1]);
  // пересчитываем размер файла в буферы, округляя вверх
  size = size / BUFSIZE_B + (size % BUFSIZE_B > 0 ? 1 : 0);
  size *= BUFSIZE;
  uint first = atoi(argv[2]);

  int fd = open(FILENAME, O_CREATE | O_WRONLY);
  if (fd < 0) {
    fprintf(1, "Error: could not open file on write\n");
    exit(1);
  }

  uint64 current = (uint64)first;
  uint64 buf[BUFSIZE];

  int status;
  for (uint i = 0; i < size; ++i) {
    buf[i % BUFSIZE] = current;
    current = next(current);
    // printf("%d: %d ", i, current);
//    printf("buffer:");
//    for (uint j = 0; j < BUFSIZE; ++j) {
//      printf(" %d", buf[j]);
//    }
//    printf("\n");

    if (i > 0 && i % BUFSIZE == BUFSIZE - 1) {
      status = write(fd, buf, BUFSIZE_B);
      printf("status = %d\n", status);
      if (status != BUFSIZE_B) {
        fprintf(1, "Error: could not write to file\n");
        close(fd);
        exit(2);
      }
    }
  }
  printf("\n");

  close(fd);

  fd = open(FILENAME, O_RDONLY);
  if (fd < 0) {
    fprintf(1, "Error: could not open file on read\n");
    exit(1);
  }

  uint processed = 0, good = 0;
  current = (uint64)first;

  status = 0;
  while (processed != size) {
    status = read(fd, buf, BUFSIZE_B);
    if (status < BUFSIZE_B) {
      fprintf(1, "Error: could not read from file\n");
      close(fd);
      exit(3);
    }
    processed += BUFSIZE;
    for (uint i = 0; i < BUFSIZE; ++i) {
      if (buf[i] == current) {
        good++;
      } else {
        printf("position %d: expected %d, got %d\n", i, current, buf[i]);
      }
      current = next(current);
    }
  }

  close(fd);

  printf("size = %d, processed = %d, good = %d\n", size, processed, good);
  if (processed == size && processed == good) {
    fprintf(1, "All good\n");
    exit(0);
  }
  fprintf(1, "Oh what the... %d bad bytes!\n", processed - good);
  exit(4);
}