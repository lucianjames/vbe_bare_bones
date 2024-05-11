; Define the multiboot2 header values
MBH2_MAGIC  equ 0xE85250D6
MBH2_ARCHI  equ 0   ; ‘0’ means 32-bit (protected) mode of i386. ‘4’ means 32-bit MIPS.
; Length is calculated later on
MBH2_CHECK  equ 0x100000000 - (MBH2_MAGIC + MBH2_ARCHI)


; Create the multiboot2 section for GRUB to find
section .multiboot2
multiboot_header:
;   The base multiboot2 required header information:
    dd MBH2_MAGIC
    dd MBH2_ARCHI
    dd multiboot_header_end - multiboot_header                  ; MBH2_LEN    
    dd MBH2_CHECK - (multiboot_header_end - multiboot_header)   ; Add MBH2_LEN into MBH2_CHECK

;       3.1.10 The framebuffer tag of Multiboot2 header
;              +-------------------+
    dw 5;      | type = 5          |
    dw 0;      | flags             |    Leaving flags/width/height/depth at 0 so that the bootloader chooses whatever it sees fit.
    dd 20;     | size = 20         |
    dd 0;      | width             |
    dd 0;      | height            |
    dd 0;      | depth             |
;              +-------------------+
;       This tag specifies the preferred graphics mode. 
;       If this tag is present bootloader assumes that the payload has framebuffer support. 
;       Note that that is only a recommended mode by the OS image. 
;       Boot loader may choose a different mode if it sees fit.

;    End Of Tags (These bytes are important!)
    dw   0, 0
    dd   0
multiboot_header_end:


section .bss
    align 16
    global stack_bottom
    stack_bottom:
        resb 16384
    global stack_top
    stack_top:


; The actual code to get the kernel up and running :D
section .text
global _start:function
_start:
    mov esp, stack_top ; Set ESP to the top of the stack section

    ; Push kernel_main args
    push ebx    ; Pointer to multiboot information structure
    push eax    ; Multiboot2 magic value

    ; Call the kernel code
    extern kernel_main
    call kernel_main

    ; Kernel exited? Halt forever :P
.haltloop:
    cli ; Clear all interrupts
    hlt ; Halt until next interrupt (most likely, there wont be one)
    jmp .haltloop ; If somehow we get an interrupt, just loop back to clearing them all and halting again.

