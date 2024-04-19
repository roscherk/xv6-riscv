#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/riscv.h"
#include "kernel/spinlock.h"
#include "kernel/logger.h"
#include "user.h"

void all(int on) {
  for (int i = 0; i < EVENTSCOUNT; ++i) {
    if (on == 1) {
      log_on(i);
    } else {
      log_off(i);
    }
  }
}

int process(int on, char* event, int ticks) {
  int event_n;
  if (strcmp(event, "syscall") == 0) {
    event_n = 0;
  } else if (strcmp(event, "trap") == 0) {
    event_n = 1;
  } else if (strcmp(event, "switch") == 0) {
    event_n = 2;
  } else if (strcmp(event, "exec") == 0) {
    event_n = 3;
  } else {
    printf("Error: unknown event type: `%s`\n", event);
    return -2;
  }
  if (on) {
    if (ticks > 0) {
      return tlog_on(event_n, ticks);
    }
    return log_on(event_n);
  }
  return log_off(event_n);
}

int main(int argc, char **argv) {
  char *usage =
      "Usage: log [on/off/-h] {-a} {syscall/trap/switch/exec} {ticks}\n";
  if (strcmp(argv[1], "-h") == 0) {
    printf(usage);
    return 0;
  }
  if (argc < 3) {
    printf("Error: expected at least 2 arguments\n");
    return -1;
  }
  if (argc > 4) {
    printf("Error: expected at most 3 arguments\n");
    return -1;
  }

  if (strcmp(argv[1], "on") == 0) {
    if (strcmp(argv[2], "-a") == 0) {
      all(1);
      return 0;
    }
    return process(1, argv[2], argc == 4 ? atoi(argv[3]) : -1);
  }
  if (strcmp(argv[1], "off") == 0) {
    if (strcmp(argv[2], "-a") == 0) {
      all(0);
      return 0;
    }
    return process(0, argv[2], -1);
  }
  printf("Error: unknown argument: `%s`\n", argv[1]);
  return -2;
}
