/* Tell the linker that we want an i386 ELF32 output file */
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)

ENTRY(_start)

PHDRS
{
    text    PT_LOAD    FLAGS((1 << 0) | (1 << 2)) ;
    rodata  PT_LOAD    FLAGS((1 << 2)) ;
    data    PT_LOAD    FLAGS((1 << 1) | (1 << 2)) ;
    dynamic PT_DYNAMIC FLAGS((1 << 1) | (1 << 2)) ;
}

SECTIONS
{
    . = 0xC0000000;

    kernel_start = .;
    kernel_text_start = .;
    .text : {
        *(.text .text.*)
    } :text
    kernel_text_end = .;

    . += CONSTANT(MAXPAGESIZE);

    kernel_rodata_start = .;
    .rodata : {
        *(.rodata .rodata.*)
    } :rodata
    kernel_rodata_end = .;

    . += CONSTANT(MAXPAGESIZE);

    kernel_data_start = .;
    .data : {
        *(.data .data.*)
    } :data

    .dynamic : {
        *(.dynamic)
    } :data :dynamic

    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    } :data
    kernel_data_end = .;

    /DISCARD/ : {
        *(.eh_frame)
        *(.note .note.*)
    }

    kernel_end = .;
}
