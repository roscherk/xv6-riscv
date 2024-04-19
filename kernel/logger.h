#define EVENTSCOUNT 4
#define BUFSIZE MSGBUFPAGES * PGSIZE

struct msg_buf {
  struct spinlock lock;
  int head, tail;
  char data[BUFSIZE];
};

struct logger {
  // количество тиков, в течение которых будем протоколировать событие
  // -1, если протоколируем всегда
  int status[EVENTSCOUNT];
  struct msg_buf buffer;
};

enum event {
  Syscall, Trap, Switch, Exec
};