%include "io64.inc"

section .data
    msg db 'Hello, world!', 0

section .text
global main
main:
    mov rbp, rsp
    PRINT_STRING msg
    NEWLINE
    xor rax, rax
    ret