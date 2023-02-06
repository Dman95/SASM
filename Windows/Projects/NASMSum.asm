section .text
extern io_get_dec, io_print_dec, io_newline
global main
main:
    mov ebp, esp ; for correct debugging
    call io_get_dec ; read first number
    mov ebx, eax ; remember first number
    call io_get_dec ; read second number
    add eax, ebx ; add first number to second number
    call io_print_dec ; print result
    call io_newline ; print newline
    xor eax, eax ; set eax to 0
    ret ; return