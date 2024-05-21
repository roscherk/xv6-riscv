#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "user/user.h"


//#define MOD ((uint64)1 << 31)
#define FILENAME "large_file"
#define BUFSIZE 8
#define BUFSIZE_B (BUFSIZE * sizeof(uint64)) // in bytes
#define ints_in(size) (size / sizeof(uint64))
#define remainder(size) (size % sizeof(uint64))

uint64 next(uint64 current) { return current * 239 + 42; }

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(1, "Usage: test_lf {size} {first}\n");
    exit(1);
  }

  uint size = atoi(argv[1]);
  uint first = atoi(argv[2]);

  int fd = open(FILENAME, O_CREATE | O_WRONLY);
  if (fd < 0) {
    fprintf(1, "Error: could not open file on write\n");
    exit(1);
  }

  uint64 current = (uint64)first;
  uint64* buf = malloc(BUFSIZE_B);

  int status;
  for (uint i = 0; i < ints_in(size); ++i) {
    buf[i % BUFSIZE] = current;
    current = next(current);

    if (i > 0 && (i + 1) % BUFSIZE == 0) {
      status = write(fd, buf, BUFSIZE_B);
      if (status != BUFSIZE_B) {
        fprintf(1, "Error: could not write to file\n");
        close(fd);
        exit(2);
      }
    }
  }

  if (size % BUFSIZE_B) {
    buf[ints_in(size) % BUFSIZE] = current;
    status = write(fd, (uint8*)buf, size % BUFSIZE_B);
    if (status != size % BUFSIZE_B) {
      fprintf(1, "Error: could not write to file\n");
      close(fd);
      exit(2);
    }
  }

  close(fd);

  fd = open(FILENAME, O_RDONLY);
  if (fd < 0) {
    fprintf(1, "Error: could not open file on read\n");
    exit(1);
  }

  uint processed = 0, good = 0;
  current = (uint64)first;

  while (processed != size) {
    uint to_read = size - processed >= BUFSIZE_B ? BUFSIZE_B : size % BUFSIZE_B;
    memset(buf, 0, BUFSIZE_B);
    status = read(fd, (uint8*)buf, to_read);
    if (status < to_read) {
      fprintf(1, "Error: could not read from file\n");
      close(fd);
      exit(3);
    }
    processed += to_read;
    for (uint i = 0; i < ints_in(to_read); ++i) {
      if (buf[i] == current) {
        good += sizeof(current);
      }
      current = next(current);
    }
    if (to_read % sizeof(current)) {
      current &= ((uint64)1 << (8 * (to_read % sizeof(current)))) - 1;
      if (buf[ints_in(to_read)] == current) {
        good += to_read % sizeof(current);
      }
    }
  }

  close(fd);

  printf("bytes written: %d, processed: %d, good: %d\n", size, processed, good);
  if (processed == size && processed == good) {
    fprintf(1, "All good\n");
    exit(0);
  }
  fprintf(1, "Oh what the... %d bad bytes!\n", processed - good);
  free(buf);
  exit(4);
}