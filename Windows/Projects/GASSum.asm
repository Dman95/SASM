.data
a: .long 0
b: .long 0
inputFormat: .asciz "%d%d"
outputFormat: .asciz "%d"
 
.extern _printf
.extern _scanf
.text
.global _main
_main:
    movl %esp, %ebp # for correct debugging
    pushl $b
    pushl $a
    pushl $inputFormat
    call _scanf
    movl a, %eax
    addl b, %eax
    pushl %eax
    pushl $outputFormat
    call _printf
    movl %ebp, %esp
    xorl %eax, %eax
    ret
    
    