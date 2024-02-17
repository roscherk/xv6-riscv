# Заглушка

.data                                      # Data section

msg_fmt: .asciz "Hello, world (printf)\n"  # Message string for printf
msg_wrt: .asciz "Hello, world (write)\n"   # Message string for write
msg_err: .asciz "Write failure (stdout)\n" # Message string for write

#define MSG_LEN 21

.text                                      # Text (code) section
.global main                               # Exportable object (label)

main:
    # Common printf-based Hello, world
    la a0, msg_fmt            # First argument - format string
    call printf               # Call printf (external symbol)

    # Write syscall-based Hello, world
    li a7, 16                 # Write syscall number
    li a0, 1                  # stdout fd (1)
    la a1, msg_wrt            # Message string address
    li a2, MSG_LEN            # Message string length
    ecall                     # Syscall

    # Check for syscall return code
    li t0, MSG_LEN            # Should return this
    bne a0, t0, failure       # If not equal go to failure

    li a7, 2                  # Exit system call
    li a0, 0                  # Exit status (success)
    ecall

failure:

    li a7, 16                 # Write syscall number
    li a0, 2                  # stderr fd (2)
    la a1, msg_err            # Error message string address
    li a2, 23                 # Error message string length
    ecall                     # Syscall

    li a7, 2                  # Exit system call
    li a0, 1                  # Exit status (failure)
    ecall                     # Syscall
