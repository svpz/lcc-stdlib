; read_input.asm
default rel
extern GetStdHandle
extern ReadConsoleA   ; better for console input
section .bss
readBytes resd 1       ; DWORD for bytes read

section .text
global read_input

STD_INPUT_HANDLE equ -10

; int read_input(char* buf, int max_len)
; rcx = buf (first parameter)
; rdx = max_len (second parameter)
read_input:
    push rbx
    push r12
    push r13
    sub rsp, 32             ; shadow space

    mov rbx, rcx            ; rbx = buf
    mov r12, rdx            ; r12 = max_len

    ; Get stdin handle
    mov ecx, STD_INPUT_HANDLE
    call GetStdHandle       ; rax = handle
    mov r13, rax            ; save handle

    ; ReadConsoleA(HANDLE, LPVOID, DWORD, LPDWORD, LPVOID)
    mov rcx, r13            ; hConsole = stdin
    mov rdx, rbx            ; lpBuffer = buf
    mov r8d, r12d           ; nNumberOfCharsToRead = max_len
    lea r9, [rel readBytes] ; lpNumberOfCharsRead
    mov qword [rsp+32], 0   ; lpReserved = NULL

    call ReadConsoleA

    ; Remove trailing newline if present
    mov eax, [rel readBytes]   ; eax = chars read
    cmp eax, 0
    je .done
    dec eax
    mov byte [rbx + rax], 0   ; overwrite last char (\n) with null terminator

.done:
    mov eax, [rel readBytes]   ; return number of characters read
    pop r13
    pop r12
    pop rbx
    add rsp, 32
    ret
