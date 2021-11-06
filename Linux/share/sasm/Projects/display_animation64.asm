%include "io64.inc"

section .data
    displayArray: times 262144 db 0xff

section .text
global CMAIN
CMAIN:
    mov rbp, rsp; for correct debugging
    ;rax = min 1 | rbx = max 510
    mov rbp, rsp
    setupDisplay 512, 512, 0, 30
    call quadrat
    call quadrat
    ret

quadrat:
    mov rax, 1
    mov rbx, 510
    mov rcx, 125
    mov rdx, 385
    
    andi:
    call clearDisplay
    
    cmp rax, 205
    jl erstesq
    mov rax, 1
    mov rbx, 510
    
    erstesq:
    
    push rax
    push rax
    push rbx
    push rax
    call bresenham
    add rsp, 32
    
    push rax
    push rax
    push rax
    push rbx
    call bresenham
    add rsp, 32
    
    push rax
    push rbx
    push rbx
    push rbx
    call bresenham
    add rsp, 32

    push rbx
    push rax
    push rbx
    push rbx
    call bresenham
    add rsp, 32
    
    ;;;;
    cmp rcx, 205
    jl zweitesq
    mov rcx, 1
    mov rdx, 510
    
    zweitesq:
    
    push rcx
    push rcx
    push rdx
    push rcx
    call bresenham
    add rsp, 32
    
    push rcx
    push rcx
    push rcx
    push rdx
    call bresenham
    add rsp, 32
    
    push rcx
    push rdx
    push rdx
    push rdx
    call bresenham
    add rsp, 32

    push rdx
    push rcx
    push rdx
    push rdx
    call bresenham
    add rsp, 32
    ;;;;
                            
    inc rax
    dec rbx
    inc rcx
    dec rdx
    
    
    updateDisplay displayArray
    jmp andi
    ret

bresenham:
;Parameter: esp-24: y-ende | esp-32: x-ende | esp-40: y-anfang | esp-48: x-anfang
;rax = dx, rbx = dy, rcx = x, rdx = y, rdi = fehler, r8 singedflag for add, r9 for tmp
;---
;swap points if xstart > xend
    push rax
    push rbx
    push rcx
    push rdx
    mov rax, [rsp+48]
    mov rbx, [rsp+64]
    cmp rax, rbx
    jge noswap
    mov [rsp+64], rax
    mov [rsp+48], rbx
    mov rax, [rsp+40]
    mov rbx, [rsp+56]
    mov [rsp+56], rax
    mov [rsp+40], rbx
noswap:
;---
;REM Initialisierungen
; dx*2; dy*2
; x = xstart
; y = ystart
; SETPIXEL x,y
; fehler = dx
;---
    ;Initialisierungen
    mov r8, 1
    mov rax, [rsp+48] ;anpassen
    mov rbx, [rsp+40]
    sub rax, [rsp+64]
    sub rbx, [rsp+56]
    mov rdi, [rsp+64]
    mov rsi, [rsp+56]
    call setPixel
    mov rcx, [rsp+64]
    mov rdx, [rsp+56]
    shl rax, 1
    shl rbx, 1
    cmp rbx, 0
    jge skip
    mov rdi, rbx
    mov rsi, 0
    call absvalue
    mov rbx, rdi
    mov r8, -1
    skip:
    mov rdi, rbx
    mov rsi, 0
    call absvalue
    cmp rdi, rax
    jg teil2
;----teil1----
    mov rdi, rax
    loop: cmp [rsp+48], rcx
    jle end
    inc rcx
    sub rdi, rbx
    cmp rdi, 0
    jge if
    add rdx, r8
    add rdi, rax
    if: push rdi
    mov rdi, rcx
    mov rsi, rdx
    call setPixel
    pop rdi
    jmp loop
    teil2: ;-------
    mov rdi, rbx
    loop2: cmp [rsp+40], rdx
    jle end
    add rdx, r8
    sub rdi, rax
    cmp rdi, 0
    jge if2
    inc rcx
    add rdi, rbx
    if2: push rdi
    mov rdi, rcx
    mov rsi, rdx
    call setPixel
    pop rdi
    jmp loop2

    end:
    mov rdi, [rsp+40]
    mov rsi, [rsp+48]
    call setPixel
    pop rdx
    pop rcx
    pop rbx
    pop rax
    ret

setPixel:
    ;x in rdi und y in rsi
    push rax
    push rbx
    push rdx
    mov rax, 512
    mov rbx, rsi
    mul rbx
    add rax, rdi ;rax = y*128 + x --> current index
    mov bl, 0x00
    mov [displayArray+rax], bl
    pop rdx
    pop rbx
    pop rax
    ret

absvalue:
    ;first parameter in rdi second in rsi -> erg in rdi
    cmp rdi, rsi
    jl signjump
    sub rdi, rsi
    ret
    signjump:
    sub rsi,rdi
    mov rdi, rsi
    ret

clearDisplay:
    ;clears displayarray
    push rax
    push rbx
    xor rax, rax
    clearDisplayloop:
    mov rbx, 0xffffffffffffffff
    mov [displayArray+rax], rbx
    add rax, 8
    cmp rax, 262144
    jl clearDisplayloop
    pop rbx
    pop rax
    ret