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
    mov rdi, inputFormat
    mov rsi, a
    mov rdx, b
    call scanf
    mov rsi, qword[a]
    add rsi, qword[b]
    mov rdi, outputFormat
    call printf
    mov rsp, rbp
    xor rax, rax
    ret
    