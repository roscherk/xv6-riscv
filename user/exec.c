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
        close(0);
        dup(pipefd[0]);
        close(pipefd[0]);
        char *argv[] = {"/wc", 0};
        exec("/wc", argv);
    }

    close(pipefd[0]);   // в родительском процессе только пишем
    char buf[buffer_size];
    int status, arg_length;
    for (int i = 1; i < argc; ++i) {
        arg_length = strlen(argv[i]);
        strcpy(buf, argv[i]);
        buf[arg_length] = '\n';
        write(pipefd[1], buf, arg_length + 1);
    }
    close(pipefd[1]);
    wait(&status);
    exit(0);
}
