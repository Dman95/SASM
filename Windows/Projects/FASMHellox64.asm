format ELF64

section '.data' writeable
    msg db 'Hello, world!', 0
    formatStr db "%s", 0

section '.text' executable
public main
extrn printf
main:
    mov rbp, rsp; for correct debugging
    sub rsp, 32
    and rsp, -16
    mov rcx, formatStr
    mov rdx, msg
    call printf
    mov rsp, rbp
    xor eax, eax
    ret
    