OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)
ENTRY(main)

SECTIONS {
    . = 0x8200;
    .text : {
        *(.text .rel.text)
    }
    .data : {
        *(.data)
    }
}
