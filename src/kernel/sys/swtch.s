[section .text]
[global switch_stack]
switch_stack:
    push ebp
    mov ebp, esp
    push ebx
    push esi
    push edi
    push ebp

    mov edi, [ebp + 8]
    mov esi, [ebp + 12]
    mov [edi], esp
    mov esp, [esi]

    pop ebp
    pop edi
    pop esi
    pop ebx
    mov esp, ebp
    pop ebp
    ret
