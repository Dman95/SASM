format ELF

section '.data' writeable
    a dd 0
    b dd 0
    inputFormat db "%d%d", 0
    outputFormat db "%d", 0

section '.text' executable
public main
extrn scanf
extrn printf
main:
    mov ebp, esp; for correct debugging
    push b
    push a
    push inputFormat
    call scanf
    mov eax, dword[a]
    add eax, dword[b]
    push eax
    push outputFormat
    call printf
    mov esp, ebp
    xor eax, eax
    ret
    