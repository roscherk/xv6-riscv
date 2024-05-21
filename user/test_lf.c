#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "user/user.h"

#define MOD ((uint64)1 << 31)
#define FILENAME "large_file"
#define BUFSIZE 8
#define BUFSIZE_B (BUFSIZE * sizeof(uint64)) // in bytes
#define ints_in(size) (size / sizeof(uint64) + (size % sizeof(uint64) ? 1 : 0))

uint64 next(uint64 current) { return (current * 239 + 42) % MOD; }

void print_buf(uint64* buf) {
  printf("buffer:");
  for (uint j = 0; j < BUFSIZE; ++j) {
    printf(" %l", buf[j]);
  }
  printf("\n");
}

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
    printf("buf[%d % %d] = %d\n", i, BUFSIZE, current);
    current = next(current);

    if (i > 0 && i % (BUFSIZE - 1) == 0) {
      print_buf(buf);
      status = write(fd, buf, BUFSIZE_B);
      if (status != BUFSIZE_B) {
        fprintf(1, "Error: could not write to file\n");
        close(fd);
        exit(2);
      }
      printf("wrote %d bytes to file\n", BUFSIZE_B);
      memset(buf, 0, BUFSIZE_B);
    }
  }

  if (size % BUFSIZE_B) {
    print_buf(buf);
    status = write(fd, buf, size % BUFSIZE_B);
    if (status != size % BUFSIZE_B) {
      fprintf(1, "Error: could not write to file\n");
      close(fd);
      exit(2);
    }
    printf("wrote (another) %d bytes to file\n", size % BUFSIZE_B);
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
    uint to_read = size - processed > BUFSIZE_B ? BUFSIZE_B : size % BUFSIZE_B;
    memset(buf, 0, BUFSIZE_B);
    status = read(fd, buf, to_read);
    printf("to_read = %d, to_read / sizeof(current) = %d\n", to_read,
           ints_in(to_read));
    print_buf(buf);
    if (status < to_read) {
      fprintf(1, "Error: could not read from file\n");
      close(fd);
      exit(3);
    }
    processed += to_read;
    for (uint i = 0; i < ints_in(to_read); ++i) {
      printf("%l == %l, ", buf[i], current);
      if (buf[i] == current) {
        good += sizeof(current);
      }
      printf("next(current) = %l, hands = %l\n", next(current), current * 239 + 42);
      current = next(current);
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