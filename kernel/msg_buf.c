#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "msg_buf.h"

// блокировать только на pr_message, на отдельные байты не надо
struct msg_buf buffer;

void msg_buf_init(void) {
  buffer.head = buffer.tail = 0;
  initlock(&buffer.lock, "msg_buf lock");
}

void write_byte(char byte) {
  if (buffer.tail == buffer.head) { // кольцо замкнулось
    buffer.head++; // освобождаем место под новый символ
    while (buffer.data[buffer.head] != '\n') { // затираем первое сообщение
      buffer.data[buffer.head++] = 0;
    }
  }
  buffer.data[buffer.tail++] = byte;
  buffer.tail %= BUFSIZE;
}

// из printf.c
static char digits[] = "0123456789abcdef";

static void printint(int xx, int base, int sign) {
  char buf[16];
  int i;
  uint x;

  if (sign && (sign = xx < 0)) {
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while ((x /= base) != 0);

  if (sign) {
    buf[i++] = '-';
  }

  while (--i >= 0) {
    write_byte(buf[i]);
  }
}

static void printptr(uint64 x) {
  int i;
  consputc('0');
  consputc('x');
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (sizeof(uint64) * 8 - 4)]);
}

void pr_msg(char *fmt, ...) {
  va_list ap;
  int i, c, locking;
  char *s;

  acquire(&buffer.lock);

  if (fmt == 0)
    panic("null fmt");

  va_start(ap, fmt);
  for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
    if (c != '%') {
      write_byte(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if (c == 0)
      break;
    switch (c) {
    case 'd':
      printint(va_arg(ap, int), 10, 1);
      break;
    case 'x':
      printint(va_arg(ap, int), 16, 1);
      break;
    case 'p':
      printptr(va_arg(ap, uint64));
      break;
    case 's':
      if ((s = va_arg(ap, char *)) == 0)
        s = "(null)";
      for (; *s; s++)
        write_byte(*s);
      break;
    case '%':
      write_byte('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      write_byte('%');
      write_byte(c);
      break;
    }
  }
  va_end(ap);

  release(&buffer.lock);
}

uint64 sys_dmesg(void) {
  // test
  for (int i = 0; i < BUFSIZE; ++i) {
    write_byte((char)i);
  }
  printf("DEBUG: current buffer =\n");
  for (int i = 0; i < BUFSIZE; ++i) {
    printf("%d ", buffer.data[i]);
  }

  for (int i = 4; i < 9; ++i) {
    write_byte(i);

    printf("DEBUG: after write buffer =\n");
    for (int j = 0; j < BUFSIZE; ++j) {
      printf("%d ", buffer.data[j]);
    }
  }
  return 0;
}
