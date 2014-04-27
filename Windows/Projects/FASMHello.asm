format ELF

section '.data' writeable
    msg db 'Hello, world!', 0
    formatStr db "%s", 0

section '.text' executable
public _main
extrn _printf
_main:
    mov ebp, esp; for correct debugging
    push msg
    push formatStr
    call _printf
    add esp, 8
    xor eax, eax
    ret
    