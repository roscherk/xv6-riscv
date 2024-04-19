#define BUFSIZE MSGBUFPAGES * PGSIZE

struct msg_buf {
  struct spinlock lock;
  int head, tail;
  char data[BUFSIZE];
};
