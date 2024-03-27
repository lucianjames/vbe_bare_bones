; Define the multiboot2 header values
MBH2_MAGIC  equ 0xE85250D6
MBH2_ARCHI  equ 0   ; ‘0’ means 32-bit (protected) mode of i386. ‘4’ means 32-bit MIPS.
; Length is calculated later on
MBH2_CHECK  equ 0x100000000 - (MBH2_MAGIC + MBH2_ARCHI)


; Create the multiboot2 section for GRUB to find
section .multiboot2
multiboot_header:
    dd MBH2_MAGIC
    dd MBH2_ARCHI
    dd multiboot_header_end - multiboot_header                  ; MBH2_LEN    
    dd MBH2_CHECK - (multiboot_header_end - multiboot_header)   ; Add MBH2_LEN into MBH2_CHECK

    ; Set video mode
    ;dw 5 ; ?
    ;dw 0 ; Flags
    ;dd 20 ; ?
    ;dd 0 ; Width
    ;dd 0 ; Height
    ;dd 32 ; Bits per pixel

    ; End Of Tags (These are important!)
    dw   0, 0
    dd   0
multiboot_header_end:


; Create a stack of the given size, allowing C code to run on the system
section .bss
    align 16
    stack_bottom:
        resb 16777216;
    stack_top:


; The actual code to get the kernel up and running :D
section .text
global _start:function ; Declate _start as a function symbol with the given symbol size
_start:
    ; Set ESP reg to top of stack (grows downwards on x86) in order for C code to be able to function
    mov esp, stack_top 

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

