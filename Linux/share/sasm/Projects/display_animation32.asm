%include "io.inc"

section .data
    displayArray: times 262144 db 0xff

section .text
global CMAIN
CMAIN:
    mov ebp, esp
    ;eax = min 1 | ebx = max 510
    setupDisplay 512, 512, 0, 30
    call quadrat
    call quadrat
    ret

quadrat:
    mov eax, 1
    mov ebx, 510
    mov ecx, 125
    mov edx, 385
    
    andi:
    call clearDisplay
    
    cmp eax, 205
    jl erstesq
    mov eax, 1
    mov ebx, 510
    
    erstesq:
    
    push eax
    push eax
    push ebx
    push eax
    call bresenham
    add esp, 16
    
    push eax
    push eax
    push eax
    push ebx
    call bresenham
    add esp, 16
    
    push eax
    push ebx
    push ebx
    push ebx
    call bresenham
    add esp, 16

    push ebx
    push eax
    push ebx
    push ebx
    call bresenham
    add esp, 16
    
    ;;;;
    cmp ecx, 205
    jl zweitesq
    mov ecx, 1
    mov edx, 510
    
    zweitesq:
    
    push ecx
    push ecx
    push edx
    push ecx
    call bresenham
    add esp, 16
    
    push ecx
    push ecx
    push ecx
    push edx
    call bresenham
    add esp, 16
    
    push ecx
    push edx
    push edx
    push edx
    call bresenham
    add esp, 16

    push edx
    push ecx
    push edx
    push edx
    call bresenham
    add esp, 16
    ;;;;
                            
    inc eax
    dec ebx
    inc ecx
    dec edx
    
    
    updateDisplay displayArray
    jmp andi
    ret

bresenham:
;Parameter: esp-24: y-ende | esp-48: x-ende | esp-56: y-anfang | esp-64: x-anfang --> 24, 28, 32, 36 -> : esp+0
;eax = dx, ebx = dy, ecx = x, edx = y, edi = fehler, esp+0 singedflag for add
;---
;swap points if xstart > xend
    push eax
    push ebx
    push ecx
    push edx
    push 1 ;r8 ersatz
    mov eax, [esp+28]
    mov ebx, [esp+36]
    cmp eax, ebx
    jge noswap
    mov [esp+36], eax
    mov [esp+28], ebx
    mov eax, [esp+24]
    mov ebx, [esp+32]
    mov [esp+32], eax
    mov [esp+24], ebx
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
    ;mov r8, 1
    mov eax, [esp+28] ;anpassen
    mov ebx, [esp+24]
    sub eax, [esp+36]
    sub ebx, [esp+32]
    mov edi, [esp+36]
    mov esi, [esp+32]
    call setPixel
    mov ecx, [esp+36]
    mov edx, [esp+32]
    shl eax, 1
    shl ebx, 1
    cmp ebx, 0
    jge skip
    mov edi, ebx
    mov esi, 0
    call absvalue
    mov ebx, edi
    push eax
    mov eax, -1
    mov [esp+4], eax
    pop eax
    skip:
    mov edi, ebx
    mov esi, 0
    call absvalue
    cmp edi, eax
    jg teil2
;----teil1----
    mov edi, eax
    loop: cmp [esp+28], ecx
    jle end
    inc ecx
    sub edi, ebx
    cmp edi, 0
    jge if
    add edx, [esp]
    add edi, eax
    if: push edi
    mov edi, ecx
    mov esi, edx
    call setPixel
    pop edi
    jmp loop
    teil2: ;-------
    mov edi, ebx
    loop2: cmp [esp+24], edx
    jle end
    add edx, [esp]
    sub edi, eax
    cmp edi, 0
    jge if2
    inc ecx
    add edi, ebx
    if2: push edi
    mov edi, ecx
    mov esi, edx
    call setPixel
    pop edi
    jmp loop2

    end:
    mov edi, [esp+24]
    mov esi, [esp+28]
    call setPixel
    pop edx
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

setPixel:
    ;x in edi und y in esi
    push eax
    push ebx
    push edx
    mov eax, 512
    mov ebx, esi
    mul ebx
    add eax, edi ;eax = y*128 + x --> current index
    mov bl, 0x00
    mov [displayArray+eax], bl
    pop edx
    pop ebx
    pop eax
    ret

absvalue:
    ;first parameter in rdi second in rsi -> erg in rdi
    cmp edi, esi
    jl signjump
    sub edi, esi
    ret
    signjump:
    sub esi,edi
    mov edi, esi
    ret

clearDisplay:
    ;clears displayarray
    push eax
    push ebx
    xor eax, eax
    clearDisplayloop:
    mov ebx, 0xffffffff
    mov [displayArray+eax], ebx
    add eax, 4
    cmp eax, 262144
    jl clearDisplayloop
    pop ebx
    pop eax
    ret

;fehler: wenn gerade nach unten