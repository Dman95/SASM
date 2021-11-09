%include "io.inc"

section .data
    msg db 'Hello, world!', 0

section .text
    global CMAIN
CMAIN:
    PRINT_STRING msg
    NEWLINE
    ret