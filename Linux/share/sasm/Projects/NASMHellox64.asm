%include "io64.inc"

section .data
    msg db 'Hello, world!', 0

section .text
global main
main:
    mov rbp, rsp ; for correct debugging
    PRINT_STRING msg ; print hello world
    NEWLINE ; print newline
    xor rax, rax ; set rax to 0
    ret ; return