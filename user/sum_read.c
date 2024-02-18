#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int read_number(int* result, char ending) {
    printf("DEBUG: entering read_number...\n");
    const unsigned int BUFFER_SIZE = 33;  // по 16 байт на числа + один байт на \0 или \n
    char buffer[BUFFER_SIZE];
    int status = -2, i;
    for (i = 0; i < BUFFER_SIZE; ++i) {
        printf("\tDEBUG: i = %d\n", i);
        if (read(0, buffer + i, 1) <= 0) {
            printf("\tDEBUG: io error\n");
            status = -1;
            break;
        }
        if (buffer[i] == '\n' || buffer[i] == ' ') {
            if (buffer[i] == ending) {
                printf("\tDEBUG: all good\n");
                status = 0;
            } else {
                printf("\tDEBUG: wrong format\n");
                status = -2;
            }
            break;
        }
        if (i == BUFFER_SIZE - 1 && buffer[i] != ending) {
            printf("\tDEBUG: buffer overflow\n");
            status = -3;
        }
        printf("\tDEBUG: buffer[%d] = %c\n", i, buffer[i]);
    }
    if (status)
        return status;
    buffer[i] = '\0';
    *result = atoi(buffer);
    printf("\tDEBUG: result = %d\n", *result);
    return status;
}

void check_status(int status) {
    switch (status) {
        case -1:
            fprintf(2, "Error: IO error\n");
            exit(1);
        case -2:
            fprintf(2, "Error: format error. Enter two integers, separated by space.\n");
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
