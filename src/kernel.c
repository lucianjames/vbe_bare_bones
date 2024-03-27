/*
    Check that the correct compiler is being used
*/
#if defined(__linux__)
    #error "This must be compiled with a cross-compiler"
#elif !defined(__i386__)
    #error "This must be compiled with an x86-elf compiler"
#endif


/*
    GCC-provided headers
*/
#include <stddef.h>
#include <stdint.h>


/*
    Custom headers
*/
#include "debugserial.h" // Allows IO over COM1 for debugging inside QEMU
#include "multiboot2-mbiparse.h" // For parsing multiboot2 info struct

/*
    Macros
*/
#define MBH2_VALID_MAGIC 0x36d76289
#define ASM_INFINITE_LOOP asm volatile ("1: jmp 1b");   // Hacky method of creating a ""breakpoint"" to look at registers etc


/*
    Kernel main.
    To pass the MBH2_MAGIC and MBH2_INFO_ADDR args, simply push ebx then eax (as long as the bootloader is behaving)
*/
void kernel_main(unsigned long MBH2_MAGIC, unsigned long MBH2_INFO_ADDR){
    /*
        Set up debugging serial communications
    */
    if(init_debug_serial() == 0){
        writestr_debug_serial("INFO: Debugging serial communications initialised\n");
    }
    
    /*
        Verify we have been booted by a multiboot2 compliant bootloader
    */
    if(MBH2_MAGIC != MBH2_VALID_MAGIC){
        writestr_debug_serial("ERR: MBH2_MAGIC IS NOT 0x36d76289\n");
    }else{
        writestr_debug_serial("INFO: Valid multiboot2 magic\n");
    }

    /*
        Parse mb2 info
    */
    parse_mb2i(MBH2_INFO_ADDR);

    /*
        Boot process "done", lets say hello!
    */
    writestr_debug_serial("\n\n======================\nHello from the kernel!\n======================\n");
}
