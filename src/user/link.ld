ENTRY(start)

USER_START = 0xA0000000;

SECTIONS
{
    . = USER_START;

    .text :
    {
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
