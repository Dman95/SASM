%include "io.inc"

section .data
    msg db 'Hello, world!', 0

section .text
global main
main:
    mov ebp, esp
    PRINT_STRING msg
    NEWLINE
    xor eax, eax
    ret