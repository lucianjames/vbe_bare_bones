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
#include "kterminal.h"

/*
    Symbols provided by the boot NASM code
*/
extern char stack_bottom[];
extern char stack_top[];


/*
    Macros
*/
#define MBH2_VALID_MAGIC 0x36d76289
#define ASM_INFINITE_LOOP asm volatile ("1: jmp 1b");   // Hacky method of creating a ""breakpoint"" to look at registers etc


/*
    Kernel main.
    To pass the mb_magic and mb2_info_struct_addr args, simply push ebx then eax (as long as the bootloader is behaving)
*/
void kernel_main(unsigned long mb_magic, unsigned long mb2_info_struct_addr){

    /*
        Set up debugging serial communications
    */
    if(init_debug_serial() == 0){
        writestr_debug_serial("INFO: Debugging serial communications initialised\n");
    }
    

    /*
        Verify we have been booted by a multiboot2 compliant bootloader
    */
    if(mb_magic != MBH2_VALID_MAGIC){
        writestr_debug_serial("ERR: mb_magic is NOT valid\n");
    }else{
        writestr_debug_serial("INFO: Valid multiboot2 magic\n");
    }


    /*
        Parse mb2 info
    */
    struct MB2TAGS mb2_info = get_mb2i_tags(mb2_info_struct_addr);


    /*
        Dump font rendering information
    */
    dump_psf_info();


    /*
        Init "kterm" to allow easy writing of debug info to the screen
    */
    kterm_init(mb2_info);
    kterm_write_newline("INFO: kterm initialised");


    /*
        Boot process "done", lets say hello!
    */
    writestr_debug_serial("\n======================\nHello from the kernel!\n======================\n\n");
    kterm_write_newline("======================");
    kterm_write_newline("Hello from the kernel!");
    kterm_write_newline("======================");


    /*
        ======= MEM DEBUGGING AND SHIT :)
    */


    /*
        Print information about stack memory
    */
    kterm_printf_newline("stack bottom = 0x%x", stack_bottom);
    kterm_printf_newline("stack top = 0x%x", stack_top);
    uint32_t stack_pointer = 0;
    asm volatile("movl %%esp,%0" : "=r"(stack_pointer));
    kterm_printf_newline("stack pointer = 0x%x", stack_pointer);


    /*
        Display mmap info - temporary testing
    */
    struct multiboot_tag* tag = (struct multiboot_tag*)(mb2_info_struct_addr+8);
    while(tag->type != MULTIBOOT_TAG_TYPE_MMAP && tag->type != MULTIBOOT_TAG_TYPE_END){
        tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7));
    }
    if(tag->type == MULTIBOOT_TAG_TYPE_MMAP){
        multiboot_memory_map_t* mmap;
        kterm_printf_newline("MMAP information:");
        for(mmap = ((struct multiboot_tag_mmap*)tag)->entries;
            (multiboot_uint8_t*)mmap < (multiboot_uint8_t*)tag + tag->size;
            mmap = (multiboot_memory_map_t*)((unsigned long)mmap + ((struct multiboot_tag_mmap*)tag)->entry_size)
        ){
            kterm_printf_newline("    base_addr = 0x%x%x, length = 0x%x%x, type = 0x%x", 
                (uint32_t)(mmap->addr >> 32), 
                (uint32_t)(mmap->addr & 0xffffffff),
                (uint32_t)(mmap->len >> 32), 
                (uint32_t)(mmap->len & 0xffffffff),
                (uint32_t)mmap->type
                );
            
        }
    }else{
        writestr_debug_serial("ERR: Couldnt find MMAP tag! Something is very wrong!");
        kterm_write_newline("ERR: Couldnt find MMAP tag! Something is very wrong!");
        return;
    }

}
