#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/procinfo.h"

int main() {
    struct procinfo buf[64];
    int res = ps_listinfo(buf, 64);
    printf("OK: %d\n", res);

    res = ps_listinfo(0, 64);
    printf("NULL: %d\n", res);

    struct procinfo small_buf[1];
    res = ps_listinfo(small_buf, 1);
    printf("SMALL BUFFER: %d\n", res);

    res = ps_listinfo((struct procinfo*)(-1), 64);
    printf("INCORRECT ADDRESS: %d\n", res);
    exit(0);
}