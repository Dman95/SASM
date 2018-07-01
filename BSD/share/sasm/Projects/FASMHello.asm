format ELF

section '.data' writeable
    msg db 'Hello, world!', 0
    formatStr db "%s", 0

section '.text' executable
public main
extrn printf
main:
    mov ebp, esp; for correct debugging
    push msg
    push formatStr
    call printf
    add esp, 8
    xor eax, eax
    ret
    