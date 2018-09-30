format ELF64

section '.data' writeable
    msg db 'Hello, world!', 0
    formatStr db "%s", 0

section '.text' executable
public main
extrn printf
main:
    mov rbp, rsp; for correct debugging
    and rsp, -16
    mov rdi, formatStr
    mov rsi, msg
    call printf
    mov rsp, rbp
    xor rax, rax
    ret
    