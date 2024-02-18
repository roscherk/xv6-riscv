#include "kernel/types.h"
#include "user/user.h"

int main(int _, char* __[]) {
    printf("%d", sum(5, 10));
    return 0;
}
