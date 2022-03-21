ENTRY(start)

KERNEL_START = 0x100000;

SECTIONS
{
    . = KERNEL_START;

    .text :
    {
        *(.multiboot)
        *(.text)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(COMMON)
        *(.bss)
    }

    .rodata :
    {
        *(.rodata*)
    }

    end = .;
}
