#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main (int argc, char** argv) {
    if (argc < 2) {
        printf("CLI: not enough arguments %d (need 1)\n", argc);
        exit(1);
    }
    int child[2];
    int parent[2];
    int status = pipe(child);
    if (status < 0) {
        printf("Error: couldn't open pipe\n");
        exit(2);
    }
    status = pipe(parent);
    if (status < 0) {
        printf("Error: couldn't open pipe\n");
        exit(2);
    }

    int mutex = mutex_create();

    if (mutex < 0) {
      printf("Error: couldn't create mutex\n");
      exit(-1);
    }

    int pid = fork();
    if (pid < 0) {
        printf("Error: couldn't fork\n");
        exit(3);
    }
    int my_pid = getpid();
    char c;

    if (pid == 0) {
        // child
        close(parent[0]);
        close(child[1]);
        while ((status = read(child[0], &c, 1)) > 0) {
            if (mutex_lock(mutex) < 0) {
                printf("Error: couldn't lock mutex (child)\n");
                exit(42);
            }
            printf("%d: received %c\n", my_pid, c);
            if (mutex_unlock(mutex) < 0) {
                printf("Error: couldn't unlock mutex (child)\n");
                exit(43);
            }
            if (write(parent[1], &c, 1) != 1) {
                printf("Error: couldn't write (child)\n");
                exit(4);
            }
        }
        close(parent[1]);
        close(child[0]);
        if (status < 0) {
            printf("Error: couldn't read (child)\n");
            exit(5);
        }
        if (mutex_release(mutex) < 0) {
            printf("Error: couldn't release mutex (child)\n");
            exit(44);
        }
        exit(0);
    }

    // parent
    close(parent[1]);
    close(child[0]);
    int arg_length = (int)strlen(argv[1]);
    if (write(child[1], argv[1], arg_length) != arg_length) {
        printf("Error: couldn't write (parent)\n");
        exit(6);
    }
    close(child[1]);

    while ((status = read(parent[0], &c, 1)) > 0) {
        if (mutex_lock(mutex) < 0) {
            printf("Error: couldn't lock mutex (parent)\n");
            exit(42);
        }
        printf("%d: received %c\n", my_pid, c);
        if (mutex_unlock(mutex) < 0) {
            printf("Error: couldn't lock mutex (parent)\n");
            exit(43);
        }
    }
    close(parent[0]);
    if (status < 0) {
        printf("Error: couldn't read (parent)\n");
        exit(7);
    }
    wait(&status);
    if (mutex_release(mutex) < 0) {
        printf("Error: couldn't release mutex (parent)\n");
        exit(44);
    }
    return 0;
}
