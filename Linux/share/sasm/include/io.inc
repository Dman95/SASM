%ifndef IO_SYS
%define IO_SYS

%macro sasmMacroFunc 0.nolist ;func for debug
    %push sasmMacroFunc
    call %$sasmMacro
    jmp %$sasmMacroE
    %$sasmMacro:
    jmp %%after_data
section .data
    %$sasmRetAddr dd 0
section .text
    %%after_data:
    push ebx
    mov ebx, dword[esp + 4]
    mov dword[%$sasmRetAddr], ebx
    mov ebx, dword[esp]
    mov dword[esp + 4], ebx
    pop ebx
    pop ebx
%endmacro

%macro sasmMacroFuncE 0.nolist ;exit
    push dword[%$sasmRetAddr]
    ret
    %$sasmMacroE:
    %pop
%endmacro

%macro CEXTERN 1.nolist
    extern %1
%endmacro
%define CMAIN main

CEXTERN printf
CEXTERN scanf
CEXTERN putchar
CEXTERN fgets
CEXTERN puts
CEXTERN fputs
CEXTERN fflush

CEXTERN get_stdin
CEXTERN get_stdout

; Make stack be 16 bytes aligned after pushing %1 bytes
%macro ALIGN_STACK 1.nolist
    enter 0, 0
    sub esp, %1
    and esp, 0xfffffff0
    add esp, %1
%endmacro

%macro UNALIGN_STACK 0.nolist
    leave
%endmacro

%macro FFLUSH_STDOUT 0.nolist
    ALIGN_STACK 4
    call get_stdout
    push eax
    call fflush
    UNALIGN_STACK
%endmacro

%macro IS_GPR 1.nolist
    %push IS_GPR
    %assign %$is_reg 0
    %assign %$reg_size 1
    %ifidni %1, ah
        %assign %$is_reg 1
    %elifidni %1, al
        %assign %$is_reg 1
    %elifidni %1, bl
        %assign %$is_reg 1
    %elifidni %1, bh
        %assign %$is_reg 1
    %elifidni %1, cl
        %assign %$is_reg 1
    %elifidni %1, ch
        %assign %$is_reg 1
    %elifidni %1, dl
        %assign %$is_reg 1
    %elifidni %1, dh
        %assign %$is_reg 1
    %elifidni %1, ax
        %assign %$is_reg 1
        %assign %$reg_size 2
    %elifidni %1, bx
        %assign %$is_reg 1
        %assign %$reg_size 2
    %elifidni %1, cx
        %assign %$is_reg 1
        %assign %$reg_size 2
    %elifidni %1, dx
        %assign %$is_reg 1
        %assign %$reg_size 2
    %elifidni %1, sp
        %assign %$is_reg 1
        %assign %$reg_size 2
    %elifidni %1, bp
        %assign %$is_reg 1
        %assign %$reg_size 2
    %elifidni %1, si
        %assign %$is_reg 1
        %assign %$reg_size 2
    %elifidni %1, di
        %assign %$is_reg 1
        %assign %$reg_size 2
    %elifidni %1, eax
        %assign %$is_reg 1
        %assign %$reg_size 4
    %elifidni %1, ebx
        %assign %$is_reg 1
        %assign %$reg_size 4
    %elifidni %1, ecx
        %assign %$is_reg 1
        %assign %$reg_size 4
    %elifidni %1, edx
        %assign %$is_reg 1
        %assign %$reg_size 4
    %elifidni %1, esp
        %assign %$is_reg 1
        %assign %$reg_size 4
    %elifidni %1, ebp
        %assign %$is_reg 1
        %assign %$reg_size 4
    %elifidni %1, esi
        %assign %$is_reg 1
        %assign %$reg_size 4
    %elifidni %1, edi
        %assign %$is_reg 1
        %assign %$reg_size 4
    %endif
%endmacro


%macro PRINT_STRING 1.nolist
    sasmMacroFunc
    IS_GPR %1
    %if %$is_reg = 1
    %error "Register as parameter is not supported"
    %endif
    %pop

%ifid %1
; variable
    pushf
    push edx
    push ecx
    push eax	

    ALIGN_STACK 0
    call get_stdout
    UNALIGN_STACK
    ALIGN_STACK 8
    push eax 
    push %1

%elifstr %1
; string literal
    pushf
    push edx
    push ecx
    push eax	

    jmp %%after_str
    %%str	db	%1, 0
%%after_str:	
    ALIGN_STACK 0
    call get_stdout
    UNALIGN_STACK
    ALIGN_STACK 8
    push eax
    push %%str

%else
; address expression
    jmp %%after_data
section .data
    %%tmp dd 0
section .text
%%after_data:
    mov dword [%%tmp], edi
    lea edi, %1

    pushf
    push edx
    push ecx
    push eax

    ALIGN_STACK 0
    call get_stdout
    UNALIGN_STACK
    ALIGN_STACK 8
    push eax
    push edi

    mov edi, dword [%%tmp]
    
%endif

    call fputs
    UNALIGN_STACK
    FFLUSH_STDOUT
    pop eax
    pop ecx
    pop edx
    popf
    sasmMacroFuncE
%endmacro

%macro NEWLINE 0.nolist
    PRINT_STRING `\n`
%endmacro

; size baseformatletter ("d", "u", "x") varname (%%fmt)
%macro ___MAKE_FORMAT_STR 3.nolist
    jmp %%after_fmt
    %if %1 = 1
        %strcat fmts "%hh" %2
        
    %elif %1 = 2
        %strcat fmts "%h" %2
    %elif %1 = 4
        %strcat fmts "%" %2
    %else
        %error "Expected numeric constant 1, 2 or 4 as 1st argument"
    %endif
    %3 db fmts, 0
    %%after_fmt:
%endmacro

; size data baseformatletter ("d", "u", "x") signextendinst (movzx, movsx)
%macro ___PRINT_NUM_COMMON 4.nolist
    ___MAKE_FORMAT_STR %1, %3, %%fmt
        
    jmp %%after_data
section .data
    %%tmp dd 0

section .text
%%after_data:

    IS_GPR %2

%if %$is_reg = 1
; register

    mov dword [%%tmp], eax    ; may be register is esp, need to take care

    %if %$reg_size = 1
        %4 eax, %2
    %elif %$reg_size = 2
        %4 eax, %2
    %else ; %$reg_size = 4
        mov eax, %2
    %endif

    pushf
    push dword [%%tmp] ; eax
    push ecx
    push edx

    ALIGN_STACK 8
    push eax

%elifid %2
; variable (hope so)
    pushf
    push eax
    push ecx
    push edx
    
%if %1 = 1
    %4 eax, byte [%2]
%elif %1 = 2
    %4 eax, word [%2]
%else ; %1 = 4
    mov eax, dword [%2]
%endif

    ALIGN_STACK 8
    push eax

%elifnum %2
    pushf
    push eax
    push ecx
    push edx

    ALIGN_STACK 8
    push %2
%elifstr %2
    pushf
    push eax
    push ecx
    push edx

    ALIGN_STACK 8
    push %2

%else
; may be, address expression?
    
    mov dword [%%tmp], eax    ; may use register esp, need to take care

    %if %1 = 1
        %4 eax, byte %2
    %elif %1 = 2
        %4 eax, word %2
    %else ; %1 = 4
        mov eax, dword %2
    %endif

    pushf
    push dword [%%tmp] ; eax
    push ecx
    push edx

    ALIGN_STACK 8
    push eax

%endif

    push %%fmt
    call printf
    UNALIGN_STACK
    FFLUSH_STDOUT

    pop edx
    pop ecx
    pop eax
    popf    

    %pop ; IS_REG
    
%endmacro

%macro PRINT_DEC 2.nolist
    sasmMacroFunc
    ___PRINT_NUM_COMMON %1, %2, "d", movsx
    sasmMacroFuncE
%endmacro

%macro PRINT_UDEC 2.nolist
    sasmMacroFunc
    ___PRINT_NUM_COMMON %1, %2, "u", movzx
    sasmMacroFuncE
%endmacro

%macro PRINT_HEX 2.nolist
    sasmMacroFunc
    ___PRINT_NUM_COMMON %1, %2, "x", movzx
    sasmMacroFuncE
%endmacro

%macro PRINT_CHAR 1.nolist
    sasmMacroFunc

    jmp %%after_data
section .data
    %%tmp dd 0

section .text
%%after_data:

    IS_GPR %1

%if %$is_reg = 1
; register

    mov dword [%%tmp], eax    ; may be register is esp, need to take care

    %if %$reg_size = 1
        movzx eax, %1
    %elif %$reg_size = 2
        movzx eax, %1
    %else ; %$reg_size = 4
        mov eax, %1
    %endif

    pushf
    push dword [%%tmp] ; eax
    push ecx
    push edx

    and eax, 0xff
    ALIGN_STACK 4
    push eax

%elifid %1
; variable (hope so)
    pushf
    push eax
    push ecx
    push edx

    movzx eax, byte [%1]
    ALIGN_STACK 4    
    push eax

%elifnum %1
    pushf
    push eax
    push ecx
    push edx

    ALIGN_STACK 4
    push (%1 & 0xff)
%elifstr %1
    pushf
    push eax
    push ecx
    push edx

; string; select only 1st byte
%substr tstr %1 1
    ALIGN_STACK 4
    push tstr

%else
; may be, address expression?
    
    mov dword [%%tmp], eax    ; may use register esp, need to take care

    movzx eax, byte %1

    pushf
    push dword [%%tmp] ; eax
    push ecx
    push edx

    ALIGN_STACK 4
    push eax

%endif

    call putchar
    UNALIGN_STACK
    FFLUSH_STDOUT

    pop edx
    pop ecx
    pop eax
    popf    

    %pop ; IS_REG
    sasmMacroFuncE
%endmacro


; size data baseformatletter ("d", "u", "x") signextendinst (movzx, movsx)
%macro ___GET_NUM_COMMON 4.nolist

    ___MAKE_FORMAT_STR %1, %3, %%fmt
        
    jmp %%after_data
section .data
    %%tmp dd 0
    %%read_tmp dd 0

section .text
%%after_data:

    IS_GPR %2

%if %$is_reg = 1
; register

    %ifidni %2, esp
        %error "Won't read to esp!" 
    %elifidni %2, sp
        %error "Won't read to sp!" 
    %endif

    %if %$reg_size < %1
        %error "Too small register for requested data"
    %endif

    pushf
    push eax
    push ecx
    push edx

; we will have to do postprocessing after scanf
    ALIGN_STACK 8
    push %%read_tmp

%elifid %2
; variable (hope so)
    pushf
    push eax
    push ecx
    push edx

    ALIGN_STACK 8    
    push %2
%elifnum %2
    %error "Incorrect parameter (number)"
%elifstr %2
    %error "Incorrect parameter (string)"
%else
; may be, address expression?
    
    mov dword [%%tmp], eax    ; may use register esp, need to take care

    lea eax, %2

    pushf
    push dword [%%tmp] ; eax
    push ecx
    push edx

    ALIGN_STACK 8
    push eax
%endif

    push %%fmt
    call scanf
    UNALIGN_STACK

    pop edx
    pop ecx
    pop eax
    popf    

%if %$is_reg = 1
; register postprocessing

%if %$reg_size = %1
    mov %2, [%%read_tmp]
%else ; extend bytes 1 -> 2, 1 -> 4, 2 -> 4
    %if %1 = 1
        %4 %2, byte [%%read_tmp]
    %else ; 2 -> 4
        %4 %2, word [%%read_tmp]
    %endif
%endif

%endif

    %pop ; IS_REG
%endmacro

%macro GET_HEX 2.nolist
    sasmMacroFunc
    ___GET_NUM_COMMON %1, %2, "x", movzx
    sasmMacroFuncE
%endmacro

%macro GET_DEC 2.nolist
    sasmMacroFunc
    ___GET_NUM_COMMON %1, %2, "d", movsx
    sasmMacroFuncE
%endmacro

%macro GET_UDEC 2.nolist
    sasmMacroFunc
    ___GET_NUM_COMMON %1, %2, "u", movzx
    sasmMacroFuncE
%endmacro


%macro GET_CHAR 1.nolist
    sasmMacroFunc

    jmp %%after_data
section .data
    %%fmt db "%c", 0
    %%tmp dd 0
    %%read_tmp db 0

section .text
%%after_data:

    IS_GPR %1

%if %$is_reg = 1
; register

    %ifidni %1, esp
        %error "Won't read to esp!" 
    %elifidni %1, sp
        %error "Won't read to sp!" 
    %elifidni %1, spl
        %error "Won't read to spl!" 
    %endif

    pushf
    push eax
    push ecx
    push edx

    ALIGN_STACK 8
; we will have to do postprocessing after scanf
    push %%read_tmp

%elifid %1
; variable (hope so)
    pushf
    push eax
    push ecx
    push edx

    ALIGN_STACK 8
    push %1
    
%elifnum %1
    %error "Incorrect parameter (number)"
%elifstr %1
    %error "Incorrect parameter (string)"
%else
; may be, address expression?
    
    mov dword [%%tmp], eax    ; may use register esp, need to take care

    lea eax, %1

    pushf
    push dword [%%tmp] ; eax
    push ecx
    push edx

    ALIGN_STACK 8
    push eax
%endif

    push %%fmt
    call scanf
    UNALIGN_STACK

    pop edx
    pop ecx
    pop eax
    popf    

%if %$is_reg = 1
; register postprocessing

%if %$reg_size = 1
    mov %1, byte [%%read_tmp]
%else ; zero extend bytes 1 -> 2, 1 -> 4
    movzx %1, byte [%%read_tmp]
%endif

%endif

    %pop ; IS_REG
    sasmMacroFuncE
%endmacro


%macro GET_STRING 2.nolist
    sasmMacroFunc

    IS_GPR %1

%if %$is_reg = 1
    %error "Incorrect 1st parameter (register)"
%endif
    

    %pop

    IS_GPR %2

%if %$is_reg = 1
    ;
%elifnum %2
    %if %2 <= 0
        %error "Second parameter must be positive"
    %endif
%else  
    %error "Second parameter must be numeric constant or register"
%endif

    %pop

%ifid %1
; variable (hope so)
    push esi
    ;count
    IS_GPR %2
    %if %$is_reg = 1 
        %if %$reg_size = 1
            movzx esi, %2
        %elif %$reg_size = 2
            movzx esi, %2
        %else ; %$reg_size = 4
            mov esi, %2
        %endif
    %else
        mov esi, %2
    %endif
    %pop
    
    pushf
    push eax
    push ecx
    push edx
    ALIGN_STACK 0
    call get_stdin
    UNALIGN_STACK
    ALIGN_STACK 12
    push eax
    push esi ; count
    push %1
%elifnum %1
    %error "Incorrect 1st parameter (number)"
%elifstr %1
    %error "Incorrect 1st parameter (string)"
%else
; may be, address expression?
    
    jmp %%after_data
section .data
    %%tmp dd 0
section .text
%%after_data:

    ; may use register esp, need to take care
    mov dword [%%tmp], edi
    lea edi, %1
    
    push esi
    ;count
    IS_GPR %2
    %if %$is_reg = 1 
        %if %$reg_size = 1
            movzx esi, %2
        %elif %$reg_size = 2
            movzx esi, %2
        %else ; %$reg_size = 4
            mov esi, %2
        %endif
    %else
        mov esi, %2
    %endif
    %pop

    pushf
    push eax
    push ecx
    push edx
    ALIGN_STACK 0
    call get_stdin
    UNALIGN_STACK
    ALIGN_STACK 12
    push eax    
    push esi ; count
    push edi
 
    mov edi, dword [%%tmp]
%endif

    call fgets
    UNALIGN_STACK

    pop edx
    pop ecx
    pop eax
    popf
    pop esi
    sasmMacroFuncE
%endmacro
%endif

