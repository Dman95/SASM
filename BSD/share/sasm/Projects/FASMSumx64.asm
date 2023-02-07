format ELF64

section '.data' writeable
    a dq 0
    b dq 0
    inputFormat db "%lld%lld", 0
    outputFormat db "%lld", 0

section '.text' executable
public main
extrn scanf
extrn printf
main:
    mov rbp, rsp; for correct debugging
    sub rsp, 32
    and rsp, -16
    mov rcx, inputFormat
    mov rdx, a
    mov r8, b
    call scanf
    mov rdx, qword[a]
    add rdx, qword[b]
    mov rcx, outputFormat
    call printf
    mov rsp, rbp
    xor rax, rax
    ret
    