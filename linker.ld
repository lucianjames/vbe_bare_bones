ENTRY(_start)

SECTIONS
{
    /* Begin putting sections at 2 megabytes, a good place for kernels to be loaded at by the bootloader */
    /* because stuff starts to break due to sections being reserved below there */
    . = 2M;

    /* Aligning all sections at multiples of 4K will make things easier later in development when paging is added */

    /* First, the multiboot header */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot2) /* The multiboot header must be within the first 8KiB of the file! So we make sure to place it right at the start */
        *(.text)
    }

    /* Read-only data */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    /* Read-write data (initialized) */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    /* Read-write data (uninitialized) and stack */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}
