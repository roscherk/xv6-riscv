#include "kernel/types.h"
#include "user/user.h"

int read_number(int* result, char ending) {
    const unsigned int BUFFER_SIZE = 10;  // 9 байт на число + 1 байт на разделитель
    char buffer[BUFFER_SIZE];
    int status = -2, i;
    for (i = 0; i < BUFFER_SIZE; ++i) {
        if (read(0, buffer + i, 1) <= 0) {
            status = -1;
            break;
        }
        if (buffer[i] == '\n' || buffer[i] == ' ') {
            if (buffer[i] == ending) {
                status = 0;
            } else {
                status = -2;
            }
            break;
        }
        if (i == BUFFER_SIZE - 1 && buffer[i] != ending) {
            status = -3;
        }
    }
    if (status)
        return status;
    buffer[i] = '\0';
    *result = atoi(buffer);
    return status;
}

void check_status(int status) {
    switch (status) {
        case -1:
            fprintf(2, "Error: IO error\n");
            exit(1);
        case -2:
            fprintf(2, "Error: format error. Enter two integers separated by a space.\n");
            exit(1);
        case -3:
            fprintf(2, "Error: buffer overflow, integer too long\n");
            exit(1);
        default:
            return;
    }
}

int main(int _, char* __[]) {
    int number1, number2, status;

    status = read_number(&number1, ' ');
    check_status(status);
    status = read_number(&number2, '\n');
    check_status(status);

    printf("DEBUG: |%d %d|\n", number1, number2);
    printf("%d\n", number1 + number2);
    exit(0);
}
