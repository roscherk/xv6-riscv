#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"
#include "logger.h"

struct logger logger;

void logger_init(void) {
  for (int i = 0; i < EVENTSCOUNT; ++i) {
    logger.status[i] = 0;
  }
  logger.buffer.head = logger.buffer.tail = 0;
  for (int i = 0; i < BUFSIZE; ++i) {
    logger.buffer.data[i] = 0;
  }
  logger.buffer.data[logger.buffer.head] = '\n';
  initlock(&logger.buffer.lock, "msg_buf lock");
}

void log_on(enum event event) {
  logger.status[event] = -1;
}

void tlog_on(enum event event, int tcks) {
  logger.status[event] = ticks + tcks;
}

void log_on_all() {
  for (int i = 0; i < EVENTSCOUNT; ++i) {
    logger.status[i] = -1;
  }
}

void tlog_on_all(int tcks) {
  for (int i = 0; i < EVENTSCOUNT; ++i) {
    logger.status[i] = ticks + tcks;
  }
}

void log_off(enum event event) {
  logger.status[event] = 0;
}

void log_off_all() {
  for (int i = 0; i < EVENTSCOUNT; ++i) {
    logger.status[i] = 0;
  }
}

void write_byte(char byte) {
  int head = logger.buffer.head, tail = logger.buffer.tail;
  char *data = logger.buffer.data;

  if (tail == head) { // кольцо замкнулось
    while (data[head] != 0 && data[head] != '\n') { // затираем первое сообщение
      data[head++] = 0;
      head %= BUFSIZE;
    }
    if (data[head] == '\n') { // дотираем
      data[head++] = 0;
      head %= BUFSIZE;
    }
  }
  data[tail++] = byte;
  tail %= BUFSIZE;

  logger.buffer.head = head; logger.buffer.tail = tail;
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
  write_byte('0');
  write_byte('x');
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4) {
    write_byte(digits[x >> (sizeof(uint64) * 8 - 4)]);
  }
}

void vlog_put(const char *fmt, va_list ap) {
  int i, c;
  char *s;

  acquire(&logger.buffer.lock);

  if (fmt == 0) {
    panic("null fmt");
  }

  write_byte('[');
  printint(ticks, 10, 0);
  write_byte(']');
  write_byte(' ');

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
  release(&logger.buffer.lock);
}
// <<< printf.c

void log_event(enum event event, ...) {
  if (logger.status[event] != -1
      && ticks > logger.status[event]) {
    return;
  }

  char* type;
  va_list params;
  va_start(params, event);
  // printf("log_event: event = %d\n", event);
  switch (event) {
  case Syscall:
    // printf("log_event: Syscall\n");
    vlog_put("syscall: caller_ID = %d, caller_name = %s, syscall_n = %d", params);
    break;
  case Trap:
    type = va_arg(params, char*);
    if (strncmp(type, "UART", strlen("UART")) == 0) {
      vlog_put("trap: device = UART, irq = %d", params);
    } else if (strncmp(type, "virtio", strlen("virtio")) == 0) {
//       printf("log_event: virtio trap\n");
      vlog_put("trap: device = virtio, irq = %d", params);
    } else if (strncmp(type, "symbol", strlen("symbol")) == 0) {
      vlog_put("trap: symbol = `%s`", params);
    } else {
      vlog_put("trap: char_code = %d", params);
    }
    break;
  case Switch:
    // printf("log_event: Switch\n");
    vlog_put("process: PID = %d, name = %s, trapframe = %p, context = %p", params);
    break;
  case Exec:
    // printf("log_event: Exec\n");
    vlog_put("exec: PID = %d, name = %s", params);
    break;
  }
  va_end(params);
}

uint64 sys_dmesg(void) {
  uint64 buf;
  int copied = 0;
  argaddr(0, &buf);

  acquire(&logger.buffer.lock);
  int head = logger.buffer.head, tail = logger.buffer.tail;
  char *data = logger.buffer.data;

  if (head == tail) { // буфер закольцевался, выдаём весь
    if (copyout(myproc()->pagetable, buf, data, BUFSIZE) < 0) {
      release(&logger.buffer.lock);
      return -1;
    }
  } else {
    while (head != tail) {
      if (copyout(myproc()->pagetable, buf + copied, &data[head], 1) < 0) {
        release(&logger.buffer.lock);
        return -1;
      }
      copied++;
      head++;
      head %= BUFSIZE;
    }
  }

  char zero = 0;
  if (copyout(myproc()->pagetable, buf + copied, &zero, 1) < 0) {
    release(&logger.buffer.lock);
    return -1;
  }

  logger.buffer.head = head; logger.buffer.tail = tail;
  release(&logger.buffer.lock);
  return 0;
}
