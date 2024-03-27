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

    ; Video mode (currently not doing anyyyy video at all, kernel still works with all these at 0 right now)
    dd 0    ; Mode type
    dd 0    ; Width
    dd 0    ; Height
    dd 0    ; Depth

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
