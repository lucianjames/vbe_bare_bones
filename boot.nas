MBH_ALIGN    equ 1 << 0
MBH_MEMINFO  equ 1 << 1
MBH_VMTINFO  equ 1 << 2
MBH_FLAGS    equ MBH_ALIGN | MBH_MEMINFO | MBH_VMTINFO
MBH_MAGIC    equ 0x1BADB002
MBH_CHECKSUM equ -(MBH_MAGIC+MBH_FLAGS)

section .multiboot
align 4
    dd MBH_MAGIC
    dd MBH_FLAGS
    dd MBH_CHECKSUM

    ; aout kludge (unused)
    dd 0,0,0,0,0

    ; VIDEO MODE
    ; 3.1.4 The graphics fields of Multiboot header
    ; All of the graphics fields are enabled by flag bit 2. They specify the preferred graphics mode. Note that that is only a recommended mode by the OS image. Boot loader may choose a different mode if it sees fit.
    ; The meaning of each is as follows:
    ;    mode_type:
    ;      Contains ‘0’ for linear graphics mode or ‘1’ for EGA-standard text mode. Everything else is reserved for future expansion. Note that the boot loader may set a text mode even if this field contains ‘0’, or set a video mode even if this field contains ‘1’.
    ;    width:
    ;      Contains the number of the columns. This is specified in pixels in a graphics mode, and in characters in a text mode. The value zero indicates that the OS image has no preference.
    ;    height:
    ;      Contains the number of the lines. This is specified in pixels in a graphics mode, and in characters in a text mode. The value zero indicates that the OS image has no preference.
    ;    depth:
    ;      Contains the number of bits per pixel in a graphics mode, and zero in a text mode. The value zero indicates that the OS image has no preference.
    dd 0    ; Mode type
    dd 0    ; Width
    dd 0    ; Height
    dd 0    ; Depth
    ; Setting all of them to 0, lets see what QEMU/GRUB throws at us

section .bss
    align 16
    stack_bottom:
        resb 16384;
    stack_top:

section .text
global _start:function ; Declate _start as a function symbol with the given symbol size
_start:
    ; Set ESP reg to top of stack (grows downwards on x86) in order for C code to be able to function
    mov esp, stack_top 

    ; Call the kernel code
    extern kernel_main
    call kernel_main

    ; Kernel exited? Halt forever.
.haltloop:
    cli ; Clear all interrupts
    hlt ; Halt until next interrupt (most likely, there wont be one)
    jmp .haltloop ; If somehow we get an interrupt, just loop back to clearing them all and halting again.

.end:
