%include "io64.inc"

section .text
global main
main:
    mov rbp, rsp ; for correct debugging
    GET_DEC 8, rax ; read first number
    GET_DEC 8, rcx ; read second number
    add rax, rcx ; add second number to first number
    PRINT_DEC 8, rax ; print result
    NEWLINE ; print newline
    xor rax, rax ; set rax to 0
    ret ; return