.data                                           # Data section

msg_read_error:     .asciz "Error: IO error\n"  # IO error message
msg_format_error:   .asciz "Error: format error. Enter two integers separated by a space.\n"  # wrong format message
out_fmt:            .asciz "%d\n"               # for result printf
debug_out_fmt:      .asciz "DEBUG: |%d %d|\n"   # for debug printf

#define BUFFER_LEN 20
#define MSG_LEN 64

buffer: .byte BUFFER_LEN

.text                                           # Text (code) section
.global main                                    # Exportable object (label)

main:
    la a0, buffer
    li a1, BUFFER_LEN
    call gets                               # gets(buffer, BUFFER_LEN)

    beq a0, zero, read_error                # if gets returns 0, then read error

    la a0, buffer
    li a1, 0x20                             # ascii code for '\n'
    call strchr                             # strchr(buffer, 0x0a)

    beq a0, zero, format_error              # if strchr returns 0, then format error

    sb zero, 0(a0)                          # replace the space with a null terminator
    addi t0, a0, 1                          # move the pointer to the beginning of the second number

    la a0, buffer
    call atoi
    mv t1, a0                               # move the first number to t1

    mv a0, t0                               # move the pointer to the beginning of the second number
    call atoi
    mv t2, a0                               # move the result to t1

    la a0, debug_out_fmt
    mv a1, t1
    mv a2, t2
    call printf                             # printf(debug_out_fmt, t1, t2)

    add t3, t1, t2                          # add two numbers, store the sum in t3
    la a0, out_fmt
    mv a1, t3                               # move the result to a0
    call printf                             # printf(msg_fmt, t0)

    li a7, 2                                # exit syscall number
    li a0, 0                                # exit code
    ecall                               # exit(0)

read_error:
    li a7, 16
    li a0, 2
    la a1, msg_read_error
    li a2, MSG_LEN
    ecall                                   # write(2, msg_read_error, MSG_LEN)

    li a7, 2
    li a0, 1
    ecall                                   # exit(1);

format_error:
    li a7, 16
    li a0, 2
    la a1, msg_format_error
    li a2, MSG_LEN
    ecall                                   # write(2, msg_format_error, MSG_LEN)

    li a7, 2
    li a0, 1
    ecall                                   # exit(1);
