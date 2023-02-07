.data
a: .long 0
b: .long 0
inputFormat: .asciz "%d%d"
outputFormat: .asciz "%d"
 
.extern printf
.extern scanf
.text
.global main
main:
    movl %esp, %ebp # for correct debugging
    pushl $b
    pushl $a
    pushl $inputFormat
    call scanf
    movl a, %eax
    addl b, %eax
    pushl %eax
    pushl $outputFormat
    call printf
    movl %ebp, %esp
    xorl %eax, %eax
    ret
    
    