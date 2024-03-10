#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char** argv) {
    int pipefd[2];
    pipe(pipefd);
    const int buffer_size = 22;
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "Error: fork() finished with error.\n");
        exit(1);
    } else if (pid == 0) {
        close(pipefd[1]);   // в дочернем процессе только читаем
        char buf[buffer_size];
        int read_status;
        while ((read_status = read(pipefd[0], buf, buffer_size)) > 0) {
            printf("%s", buf);
        }
        close(pipefd[0]);
        if (read_status < 0) {
            write(2, "Error: read(...) error\n", 14);
            exit(2);
        }
        exit(0);
    }

    close(pipefd[0]);   // в родительском процессе только пишем
    int status, arg_length;
    for (int i = 1; i < argc; ++i) {
        arg_length = strlen(argv[i]);
        if (write(pipefd[1], argv[i], arg_length) != arg_length) {
            write(2, "Error: write(...) error\n", 25);
            exit(2);
        }
        if (write(pipefd[1], "\n", 1) != 1) {
            write(2, "Error: write(...) error\n", 25);
            exit(2);
        }
    }
    close(pipefd[1]);
    wait(&status);
    exit(0);
}
