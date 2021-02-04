%include "io64.inc"

section .data
    msg db 'Hello, world!', 0
    msg2 db 'adadd', 0

section .text
    global CMAIN
CMAIN:
    mov rbp, rsp; for correct debugging
    mov rbp, rsp
    PRINT_STRING msg
    mov rbp, rsp
    mov ebp, esp
    PRINT_STRING msg
    NEWLINE
    NEWLINE
    PRINT_STRING msg2
    xor eax, eax
    ret