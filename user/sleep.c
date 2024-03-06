#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char** argv) {
    // Usage: sleep [-b]
    // defaults to variant a

    int go_b = 0;
    if (argc == 2 && strcmp(argv[1], "-b")) go_b = 1;

    int pid = fork();
    if (pid < 0) {   // дочерний процесс завершился с ошибкой
        fprintf(2, "Error: fork() finished with error.\n");
        exit(2);
    } else if (pid == 0) { // в дочернем процессе
        sleep(42);
        exit(1);
    }

    int parent_pid = getpid();
    printf("Parent PID = \t%d\nChild PID = \t%d\n...\n", parent_pid, pid);

    if (go_b) {
        kill(pid);
    }

    int exit_code;
    int cpid = wait(&exit_code);
    printf("Ended PID: %d\nExit code: %d\n", cpid, exit_code);
    exit(0);
}