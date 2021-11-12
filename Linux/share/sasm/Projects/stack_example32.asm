%include "io.inc"

section .text
global CMAIN
CMAIN:
    mov ebp, esp; for correct debugging
    mov eax, 0xffff1111
    push eax
    push 0
    call example_function
    pop ax
    pop ax
    pop eax
    ret
    
example_function:
    ret
