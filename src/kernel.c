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
#include "graphics.h" // Some crappy graphics testing

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
        writestr_debug_serial("ERR: MBH2_MAGIC is NOT valid\n");
    }else{
        writestr_debug_serial("INFO: Valid multiboot2 magic\n");
    }


    /*
        Parse mb2 info
    */
    struct MBI2_INFO mbi2_info_struct = parse_mb2i(MBH2_INFO_ADDR);


    /*
        Dump font rendering information
    */
    dump_psf_info();

    /*
        Boot process "done", lets say hello!
    */
    writestr_debug_serial("\n======================\nHello from the kernel!\n======================\n\n");
    draw_psf_str(mbi2_info_struct, 0, 0, "======================");
    draw_psf_str(mbi2_info_struct, 9, 0, "Hello from the kernel!");
    draw_psf_str(mbi2_info_struct, 18, 0, "======================");

}
