[section .text]
[global start]
[extern main]
start:
    call main

    mov ebx, eax
    mov eax, 8
    int 0x80

    jmp $

[global _syscall]
_syscall:
    push ebp
    mov ebp, esp
    mov eax, [ebp +  8]
    mov ebx, [ebp + 12]
    mov ecx, [ebp + 16]
    mov edx, [ebp + 20]
    mov edi, [ebp + 24]
    mov esi, [ebp + 28]
    mov ebp, [ebp + 32]
    int 0x80
    pop ebp
    ret
