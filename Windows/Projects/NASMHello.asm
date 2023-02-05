section .data
    msg db 'Hello, world!', 0

section .text
extern io_print_string, io_newline
global main
main:
    mov ebp, esp
    mov eax, msg
    call io_print_string
    call io_newline
    xor eax, eax
    ret