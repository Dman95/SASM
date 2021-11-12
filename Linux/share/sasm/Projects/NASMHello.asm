%include "io.inc"

section .data
    msg db 'Hello, world!', 0

section .text
    global CMAIN
CMAIN:
    mov ebp, esp; for correct debugging
    PRINT_STRING msg
    NEWLINE
    ret