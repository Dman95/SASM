format ELF

section '.data' writeable
    a dd 0
    b dd 0
    inputFormat db "%d%d", 0
    outputFormat db "%d", 0

section '.text' executable
public _main
extrn _scanf
extrn _printf
_main:
    mov ebp, esp; for correct debugging
    push b
    push a
    push inputFormat
    call _scanf
    mov eax, dword[a]
    add eax, dword[b]
    push eax
    push outputFormat
    call _printf
    mov esp, ebp
    xor eax, eax
    ret
    