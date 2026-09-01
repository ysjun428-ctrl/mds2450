ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x30000000, LENGTH = 64M
}

SECTIONS
{
    . = ORIGIN(RAM);

    .text : ALIGN(4)
    {
        KEEP(*(.text.start))
        *(.text*)
    } > RAM

    .rodata : ALIGN(4)
    {
        *(.rodata*)
    } > RAM

    .data : ALIGN(4)
    {
        *(.data*)
    } > RAM

    .bss (NOLOAD) : ALIGN(8)
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > RAM

    /* ARM926 L1 Translation Table = 4096 entries x 4 bytes = 16 KiB. */
    .ttb (NOLOAD) : ALIGN(16384)
    {
        __ttb_start__ = .;
        . += 16384;
        __ttb_end__ = .;
    } > RAM

    . = ALIGN(8);
    __image_end__ = .;

    /* Keep stack near top of the 64 MiB SDRAM, inside original Area 3. */
    __stack_top__ = ORIGIN(RAM) + LENGTH(RAM) - 0x1000;

    /DISCARD/ :
    {
        *(.comment*)
        *(.note*)
        *(.ARM.exidx*)
        *(.ARM.extab*)
    }
}
