default rel
extern GetStdHandle
extern WriteFile

section .bss
written resq 1
int_buf resb 32        ; buffer for integer string (enough for int64)

section .text
global print
global print_int
global print_char    ; added

STD_OUTPUT_HANDLE equ -11

; -----------------------------
; void print(const char* msg)
; rcx = msg (Windows x64 calling convention)
; -----------------------------
print:
    sub rsp, 40              ; Shadow space + alignment
    mov rbx, rcx             ; Save msg pointer

    ; Get stdout handle
    mov ecx, STD_OUTPUT_HANDLE
    call GetStdHandle
    mov r12, rax             ; Save handle in r12

    ; Calculate string length
    mov rdx, rbx
    xor r8d, r8d
.print_len_loop:
    cmp byte [rdx + r8], 0
    je .print_len_done
    inc r8d
    jmp .print_len_loop
.print_len_done:

    ; WriteFile(handle, buffer, length, &written, NULL)
    mov rcx, r12             ; hFile
    mov rdx, rbx             ; lpBuffer
    mov r8d, r8d             ; length
    lea r9, [rel written]    ; lpNumberOfBytesWritten
    mov qword [rsp+32], 0    ; lpOverlapped = NULL
    call WriteFile

    add rsp, 40
    ret

; -----------------------------
; void print_int(long long n)
; rcx = n (64-bit signed integer)
; -----------------------------
print_int:
    sub rsp, 40              ; Shadow space + alignment

    mov rax, rcx             ; Move input to rax for division
    lea rdi, [rel int_buf+31] ; Point to end of buffer
    mov byte [rdi], 0        ; Null terminator
    dec rdi

    ; Handle zero case
    test rax, rax
    jne .pi_check_sign
    mov byte [rdi], '0'
    mov rsi, rdi             ; Start of string for printing
    jmp .pi_print

.pi_check_sign:
    xor rbx, rbx             ; Clear sign flag
    test rax, rax
    jge .pi_positive

    ; Handle negative number
    neg rax
    mov rbx, 1               ; Set sign flag

.pi_positive:
    ; Convert integer to string (reverse order)
.pi_convert_loop:
    xor rdx, rdx             ; Clear remainder
    mov r10, 10
    div r10                  ; RAX = quotient, RDX = remainder
    add dl, '0'
    mov [rdi], dl
    dec rdi
    test rax, rax
    jnz .pi_convert_loop

    ; Add minus sign if negative
    test rbx, rbx
    je .pi_after_sign
    mov byte [rdi], '-'
    dec rdi
.pi_after_sign:
    inc rdi
    mov rsi, rdi             ; rsi = start of string

.pi_print:
    ; Get stdout handle
    mov ecx, STD_OUTPUT_HANDLE
    call GetStdHandle
    mov r12, rax             ; Save handle

    ; Calculate string length
    xor r8, r8
.pi_len_loop:
    cmp byte [rsi + r8], 0
    je .pi_len_done
    inc r8
    jmp .pi_len_loop
.pi_len_done:

    ; WriteFile(handle, buffer, length, &written, NULL)
    mov rcx, r12             ; hFile
    mov rdx, rsi             ; lpBuffer
    mov r8, r8               ; length
    lea r9, [rel written]    ; lpNumberOfBytesWritten
    mov qword [rsp+32], 0    ; lpOverlapped = NULL
    call WriteFile

    add rsp, 40
    ret


; -----------------------------
; void print_char(char c)
; rcx = character (lowest byte)
; -----------------------------
print_char:
    sub rsp, 40              ; shadow space + alignment

    mov r12b, cl             ; save character
    lea rsi, [rel int_buf]   ; rsi points to buffer
    mov byte [rsi], r12b     ; store character
    mov byte [rsi+1], 0      ; null terminator

    ; Get stdout handle
    mov ecx, STD_OUTPUT_HANDLE
    call GetStdHandle
    mov r12, rax             ; Save handle in r12

    ; Calculate string length (always 1, but for consistency)
    mov r8d, 1

    ; WriteFile(handle, buffer, length, &written, NULL)
    mov rcx, r12             ; hFile
    mov rdx, rsi             ; lpBuffer
    mov r8d, r8d             ; length
    lea r9, [rel written]    ; lpNumberOfBytesWritten
    mov qword [rsp+32], 0    ; lpOverlapped = NULL
    call WriteFile

    add rsp, 40
    ret

