.data
msg:
    .asciz "Hello, world!\n"
 
.extern printf
.extern flush
.text
.global main # entry point
main:
    movq %rsp, %rbp #for correct debugging
    andq $-16, %rsp
    movq $msg, %rdi
    call printf
    movq %rbp, %rsp
    xorq %rax, %rax
    ret
    