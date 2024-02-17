#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int read_number(int* result) {
    const unsigned int BUFFER_SIZE = 33;  // по 16 байт на числа + один байт на \0
    char buffer[BUFFER_SIZE];
    int read_success = 0, i;
    for (i = 0; i + 1 < BUFFER_SIZE; ++i) {
        if (read(0, buffer + i, 1) <= 0) {
            read_success = -1;
            break;
        }
        if (buffer[i] == '\n' || buffer[i] == ' ') {
            break;
        }
    }
    if (i == 0)  // пустая строка
        return -2;
    buffer[i] = '\0';
    if (read_success == -1)
        return -1;
    *result = atoi(buffer);
    return 0;
}

void check_status(int status) {
    switch (status) {
        case -1:
            fprintf(2, "Error: IO error\n");
            exit(1);
        case -2:
            fprintf(2, "Error: format error. Enter two integers, separated by space.\n");
            exit(1);
        default:
            return;
    }
}

int main(int argc, char* argv[]) {
    int number1, number2, status;

    status = read_number(&number1);
    check_status(status);
    status = read_number(&number2);
    check_status(status);
    printf("%d + %d = %d\n", number1, number2, number1 + number2);
    exit(0);
}
