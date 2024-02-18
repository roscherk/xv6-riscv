#include "kernel/types.h"
#include "user/user.h"

int get_numbers(int* number1, int* number2) {
    printf("DEBUG: entering get_numbers...\n");
    const unsigned int BUFFER_SIZE = 20;  // по 9 байт на числа + 2 байта на пробел и \n
    char buffer[BUFFER_SIZE];
    int status = -2, i;
    gets(buffer, BUFFER_SIZE);
    for (i = 0; i < BUFFER_SIZE; ++i) {
        printf("\tDEBUG: i = %d\n", i);
        if (buffer[i] == ' ') {
            if (status == -2) {
                *number1 = atoi(buffer);
                status = i + 1;
                if (status > BUFFER_SIZE / 2) {
                    status = -3;
                    break;
                }
                printf("\tDEBUG: number1 = %d, status = %d\n", *number1, status);
            } else {
                status = -2;
                break;
            }
        } else if (buffer[i] == '\n' && status > 0) {
            printf("\tDEBUG: buffer[status] = %c\n", buffer[status]);
            *number2 = atoi(buffer + status);
            status = 0;
            printf("\tDEBUG: number2 = %d, status = %d\n", *number2, status);
            break;
        }
        if (i == BUFFER_SIZE - 1) {
            printf("\tDEBUG: buffer overflow\n");
            status = -3;
        }
        printf("\tDEBUG: buffer[%d] = %c\n", i, buffer[i]);
    }
    if (status)
        return status;
    buffer[i] = '\0';
    printf("\tDEBUG: number1 = %d, number2 = %d\n", *number1, *number2);
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

    status = get_numbers(&number1, &number2);
    check_status(status);

    printf("DEBUG: |%d %d|\n", number1, number2);
    printf("%d\n", number1 + number2);
    exit(0);
}
