section .data
    msg db 'Hello, world!', 0

section .text
extern io_print_string, io_newline
global main
main:
    mov ebp, esp ; for correct debugging
    mov eax, msg ; place pointer to hello world message to eax
    call io_print_string ; print hello world
    call io_newline ; print newline
    xor eax, eax ; set eax to 0
    ret ; return