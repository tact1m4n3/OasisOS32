%macro ISR_ERR 1
[global _isr%1]
_isr%1:
    cli
    push dword %1
    jmp int_common
%endmacro

%macro ISR_NO_ERR 1
[global _isr%1]
_isr%1:
    cli
    push dword 0
    push dword %1
    jmp int_common
%endmacro

%macro IRQ 1
[global _irq%1]
_irq%1:
    cli
    push dword 0
    push dword %1 + 0x20
    jmp int_common
%endmacro

[section .text]
[extern int_handler]
int_common:
    pusha

    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call int_handler
    add esp, 4
    mov esp, eax

    pop gs
    pop fs
    pop es
    pop ds
    
    popa
    add esp, 8

    sti
    iret

ISR_NO_ERR  0
ISR_NO_ERR  1
ISR_NO_ERR  2
ISR_NO_ERR  3
ISR_NO_ERR  4
ISR_NO_ERR  5
ISR_NO_ERR  6
ISR_NO_ERR  7
ISR_ERR     8
ISR_NO_ERR  9
ISR_ERR     10
ISR_ERR     11
ISR_ERR     12
ISR_ERR     13
ISR_ERR     14
ISR_NO_ERR  15
ISR_NO_ERR  16
ISR_ERR     17
ISR_NO_ERR  18
ISR_NO_ERR  19
ISR_NO_ERR  20
ISR_ERR     21
ISR_NO_ERR  22
ISR_NO_ERR  23
ISR_NO_ERR  24
ISR_NO_ERR  25
ISR_NO_ERR  26
ISR_NO_ERR  27
ISR_NO_ERR  28
ISR_ERR     29
ISR_ERR     30
ISR_NO_ERR  31
ISR_NO_ERR  128

IRQ  0
IRQ  1
IRQ  2
IRQ  3
IRQ  4
IRQ  5
IRQ  6
IRQ  7
IRQ  8
IRQ  9
IRQ  10
IRQ  11
IRQ  12
IRQ  13
IRQ  14
IRQ  15
