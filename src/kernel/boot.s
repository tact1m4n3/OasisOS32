MBOOT_ALIGN equ 1<<0
MBOOT_MEMINFO equ 1<<1
MBOOT_MAGIC equ 0x1BADB002
MBOOT_FLAGS equ MBOOT_ALIGN | MBOOT_MEMINFO
MBOOT_CHECKSUM equ -(MBOOT_MAGIC + MBOOT_FLAGS)

[section .multiboot]
align 8
header_start:
    dd MBOOT_MAGIC
    dd MBOOT_FLAGS
    dd MBOOT_CHECKSUM
header_end:

[section .text]
[global start]
[extern kernel_main]
start:
    cli

    mov esp, 0x80000

    push 0
    popf

    push ebx
    push eax
    call kernel_main
    add esp, 8

    jmp $
