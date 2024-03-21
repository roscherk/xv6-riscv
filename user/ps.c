#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/procinfo.h"

int main() {
    int buffer_size = 8;
    int success = 0;
    int processes;
    while (!success) {
        struct procinfo buffer[buffer_size];
        processes = ps_listinfo(buffer, buffer_size);
        if (processes < 0) {
            fprintf(2, "Error: ps_listinfo(buffer, %d) error", buffer_size);
            exit(-1);
        }
        if (processes <= buffer_size) {
            success = 1;
            for (int i = 0; i < processes; ++i) {
                char* state_str = "";
                switch (buffer[i].state) {
                    case MY_UNUSED:
                        state_str = "MY_UNUSED";
                        break;
                    case MY_USED:
                        state_str = "MY_USED";
                        break;
                    case MY_SLEEPING:
                        state_str = "MY_SLEEPING";
                        break;
                    case MY_RUNNABLE:
                        state_str = "MY_RUNNABLE";
                        break;
                    case MY_RUNNING:
                        state_str = "MY_RUNNING";
                        break;
                    case MY_ZOMBIE:
                        state_str = "MY_ZOMBIE";
                        break;
                }
                printf("Name: `%s`, PID: %d, Parent ID: %d, State: %s\n", buffer[i].name, buffer[i].pid,
                       buffer[i].parent_id, state_str);
            }
        }
        buffer_size *= 2;
    }
    exit(0);
}
