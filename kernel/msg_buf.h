//#define BUFSIZE MSGBUFPAGES * PGSIZE
#define BUFSIZE MSGBUFPAGES * 2

struct msg_buf {
  struct spinlock lock;
  int head, tail;
  char data[BUFSIZE];
};
