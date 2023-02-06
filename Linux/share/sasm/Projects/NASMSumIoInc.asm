%include "io.inc"

section .text
global main
main:
    mov ebp, esp ; for correct debugging
    GET_DEC 4, eax ; read first number
    GET_DEC 4, ecx ; read second number
    add eax, ecx ; add second number to first number
    PRINT_DEC 4, eax ; print result
    NEWLINE ; print newline
    xor eax, eax ; set eax to 0
    ret ; return