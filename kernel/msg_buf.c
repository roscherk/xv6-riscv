#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"
#include "msg_buf.h"

// блокировать только на pr_message, на отдельные байты не надо
struct msg_buf buffer;

void msg_buf_init(void) {
  buffer.head = buffer.tail = 0;
  buffer.data[buffer.head] = '\n';
  initlock(&buffer.lock, "msg_buf lock");
}

void write_byte(char byte) {
//  printf("DEBUG(0): byte = %d, head = %d, tail = %d\n", byte, buffer.head, buffer.tail);
  if (buffer.tail == buffer.head) { // кольцо замкнулось
//    printf("DEBUG(1):\t     buffer.tail == buffer.head\n");
//    buffer.head++; // освобождаем место под новый символ
//    buffer.head %= BUFSIZE;
//    printf("DEBUG(2):\t     head = %d, tail = %d\n", buffer.head, buffer.tail);
    while (buffer.data[buffer.head] != 0 && buffer.data[buffer.head] != '\n') { // затираем первое сообщение
      buffer.data[buffer.head++] = 0;
      buffer.head %= BUFSIZE;
//      printf("DEBUG(3):\t     head = %d, tail = %d\n", buffer.head, buffer.tail);
    }
  }
  buffer.data[buffer.tail++] = byte;
  buffer.tail %= BUFSIZE;
//  printf("DEBUG(4):\t     head = %d, tail = %d\n", buffer.head, buffer.tail);
}

// >>> printf.c
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
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4) {
    consputc(digits[x >> (sizeof(uint64) * 8 - 4)]);
  }
}

void pr_msg(const char *fmt, ...) {
  va_list ap;
  int i, c;
  char *s;

  acquire(&buffer.lock);

  if (fmt == 0) {
    panic("null fmt");
  }

  write_byte('[');
  acquire(&tickslock);
  printint(ticks, 10, 0);
  release(&tickslock);
  write_byte(']');
  write_byte(' ');

  va_start(ap, fmt);
  for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
    if (c != '%') {
      write_byte(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if (c == 0) {
      break;
    }
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
      if ((s = va_arg(ap, char *)) == 0) {
        s = "(null)";
      }
      for (; *s; s++) {
        write_byte(*s);
      }
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
  write_byte('\n');
  release(&buffer.lock);
}
// <<< printf.c

uint64 sys_dmesg(void) {
  uint64 buf;
  int copied = 0;
  argaddr(0, &buf);

  printf("DEBUG(5): head = %d, tail = %d\n", buffer.head, buffer.tail);

  acquire(&buffer.lock);

  if (buffer.head == buffer.tail) { // буфер закольцевался, выдаём весь
    if (copyout(myproc()->pagetable, buf, buffer.data, BUFSIZE) < 0) {
      release(&buffer.lock);
      return -1;
    }
  } else {
    while (buffer.head != buffer.tail) {
      printf("DEBUG(6): copied = %d, head = %d, tail = %d\n", copied, buffer.head, buffer.tail);
      if (copyout(myproc()->pagetable, buf + copied, &buffer.data[buffer.head], 1) < 0) {
        release(&buffer.lock);
        return -1;
      }
      copied++;
      buffer.head++;
      buffer.head %= BUFSIZE;
    }
  }
  printf("DEBUG(7): итс окэй, copied = %d\n", copied);


  char zero = 0;
  if (copyout(myproc()->pagetable, buf + copied, &zero, 1) < 0) {
    printf("Прощай, немытая Россия...\n");
    release(&buffer.lock);
    return -1;
  }
  printf("DEBUG(8): итс окэй!\n", copied);

  release(&buffer.lock);
  printf("DEBUG(9): итс окэй!!!\n", copied);
  return 0;
}
