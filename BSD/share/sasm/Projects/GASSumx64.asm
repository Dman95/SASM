.data
a: .quad 0
b: .quad 0
inputFormat: .asciz "%lld%lld"
outputFormat: .asciz "%lld"
 
.extern printf
.extern scanf
.text
.global main
main:
    movq %rsp, %rbp # for correct debugging
    subq $32, %rsp
    andq $-16, %rsp
    movq $inputFormat, %rdi
    movq $a, %rsi
    movq $b, %rdx
    call scanf
    movq (a), %rsi
    addq (b), %rsi
    movq $outputFormat, %rdi
    call printf
    movq %rbp, %rsp
    xorq %rax, %rax
    ret
    
    