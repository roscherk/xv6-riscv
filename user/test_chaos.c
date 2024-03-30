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
    int pid = fork();
    if (pid < 0) {
        printf("Error: couldn't fork\n");
        exit(3);
    }
    int my_pid = getpid();
    char c;
    int read_status;

    if (pid == 0) {
        // child
        close(parent[0]);
        close(child[1]);
        while ((read_status = read(child[0], &c, 1)) > 0) {
            printf("%d: received %c\n", my_pid, c);
            if (write(parent[1], &c, 1) != 1) {
                printf("Error: couldn't write (child)");
                exit(4);
            }
        }
        close(parent[1]);
        close(child[0]);
        if (read_status < 0) {
            printf("Error: couldn't read (child)");
            exit(5);
        }
        exit(0);
    }

    // parent
    close(parent[1]);
    close(child[0]);
    int arg_length = (int)strlen(argv[1]);
    if (write(child[1], argv[1], arg_length) != arg_length) {
        printf("Error: couldn't write (child)");
        exit(6);
    }
    close(child[1]);

    while ((read_status = read(parent[0], &c, 1)) > 0)
        printf("%d: received %c\n", my_pid, c);
    close(parent[0]);
    if (read_status < 0) {
        printf("Error: couldn't read (parent)");
        exit(7);
    }
    wait(&status);
    return 0;
}
