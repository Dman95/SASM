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
    movq $inputFormat, %rcx
    movq $a, %rdx
    movq $b, %r8
    call scanf
    movq (a), %rdx
    addq (b), %rdx
    movq $outputFormat, %rcx
    call printf
    movq %rbp, %rsp
    xorq %rax, %rax
    ret
    
    