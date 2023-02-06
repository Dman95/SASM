%include "io.inc"

section .data
    msg db 'Hello, world!', 0

section .text
global main
main:
    mov ebp, esp ; for correct debugging
    PRINT_STRING msg ; print hello world
    NEWLINE ; print newline
    xor eax, eax ; set eax to 0
    ret ; return