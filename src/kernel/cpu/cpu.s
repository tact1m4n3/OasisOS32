[section .text]
[global cli]
cli:
    cli
    ret

[global sti]
sti:
    sti
    ret

[global load_gdt]
load_gdt:
    push ebp
    mov ebp, esp

    mov ebx, [ebp + 8]
    lgdt [ebx]
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x08:load_cs
load_cs:
    mov ax, 0x2B
    ltr ax

    mov esp, ebp
    pop ebp
    ret

[global load_idt]
load_idt:
    push ebp
    mov ebp, esp

    mov ebx, [ebp + 8]
    lidt [ebx]
    
    mov esp, ebp
    pop ebp
    ret
