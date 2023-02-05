.data
msg:
    .asciz "Hello, world!\n"
 
.extern _printf
.text
.global _main # entry point
_main:
    movl %esp, %ebp # for correct debugging
    pushl $msg
    call _printf
    addl $4, %esp
    xorl %eax, %eax
    ret
    