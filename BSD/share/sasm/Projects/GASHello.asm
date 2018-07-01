.data
msg:
    .asciz "Hello, world!\n"
 
.extern printf
.text
    .global main # entry point
main:
    movl %esp, %ebp # for correct debugging
    pushl $msg
    call printf
    addl $4, %esp
    xorl %eax, %eax
    ret
    