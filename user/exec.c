#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char** argv) {
    int pipefd[2];
    int status;
    pipe(pipefd);
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "Error: fork() finished with error.\n");
        exit(1);
    } else if (pid == 0) {
        close(pipefd[1]);   // в дочернем процессе только читаем
        close(0);
        dup(pipefd[0]);
        close(pipefd[0]);
        char *argv[] = {"/wc", 0};
        if (exec("/wc", argv) < 0) {
            fprintf(2, "Error: exec(...) finished with error.\n");
            exit(1);
        }
    }

    close(pipefd[0]);   // в родительском процессе только пишем
    int arg_length;
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
