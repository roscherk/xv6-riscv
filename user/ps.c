#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/procinfo.h"

int main() {
    int buffer_size = 8;
    int success = 0;
    int processes = 0;
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
                    case MY_USED:
                        state_str = "MY_USED";
                    case MY_SLEEPING:
                        state_str = "MY_SLEEPING";
                    case MY_RUNNABLE:
                        state_str = "MY_RUNNABLE";
                    case MY_RUNNING:
                        state_str = "MY_RUNNING";
                    case MY_ZOMBIE:
                        state_str = "MY_ZOMBIE";
                }
                printf("Name: `%s`, PID: %d, State: %s\n", buffer[i].name, buffer[i].pid, state_str);
            }
        }
        buffer_size *= 2;
    }
    exit(0);
}
