#include "multiboot2-mbiparse.h"


void parse_mb2i(unsigned long addr){
    writestr_debug_serial("INFO: Attempting to parse multiboot tags\n");
    // Assume addr already validated
    struct multiboot_tag* tag = (struct multiboot_tag*)(addr+8); // Not sure what the significance of +8 is rn
    while(tag->type != MULTIBOOT_TAG_TYPE_END){
        switch (tag->type){
            case MULTIBOOT_TAG_TYPE_CMDLINE:
                writestr_debug_serial("INFO: FOUND TAG MULTIBOOT_TAG_TYPE_CMDLINE\n");
                writestr_debug_serial(" Command line = ");
                writestr_debug_serial(((struct multiboot_tag_string*)tag)->string);
                writestr_debug_serial("\n");
                break;
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                writestr_debug_serial("INFO: FOUND TAG MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME\n");
                writestr_debug_serial(" Boot loader name = ");
                writestr_debug_serial(((struct multiboot_tag_string*)tag)->string);
                writestr_debug_serial("\n");
                break;
            case MULTIBOOT_TAG_TYPE_MODULE:
                writestr_debug_serial("INFO: FOUND TAG MULTIBOOT_TAG_TYPE_MODULE\n");
                writestr_debug_serial(" Module at 0x");
                writeuint_debug_serial(((struct multiboot_tag_module *) tag)->mod_start, 16);
                writeuint_debug_serial("-0x");
                writeuint_debug_serial(((struct multiboot_tag_module *) tag)->mod_end, 16);
                writestr_debug_serial(". Command line ");
                writestr_debug_serial(((struct multiboot_tag_module *) tag)->cmdline);
                writestr_debug_serial("\n");
                break;
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
                writestr_debug_serial("INFO: FOUND TAG MULTIBOOT_TAG_TYPE_BASIC_MEMINFO\n");
                writestr_debug_serial(" mem_lower = ");
                writeuint_debug_serial(((struct multiboot_tag_basic_meminfo*)tag)->mem_lower, 10);
                writestr_debug_serial("KB\n mem_upper = ");
                writeuint_debug_serial(((struct multiboot_tag_basic_meminfo*)tag)->mem_upper, 10);
                writestr_debug_serial("KB\n");
                break;
            case MULTIBOOT_TAG_TYPE_BOOTDEV:
                writestr_debug_serial("INFO: FOUND TAG MULTIBOOT_TAG_TYPE_BOOTDEV\n");
                writestr_debug_serial(" Boot device 0x");
                writeuint_debug_serial(((struct multiboot_tag_bootdev *) tag)->biosdev, 16);
                writestr_debug_serial(", ");
                writeuint_debug_serial(((struct multiboot_tag_bootdev *) tag)->slice, 10);
                writestr_debug_serial(", ");
                writeuint_debug_serial(((struct multiboot_tag_bootdev *) tag)->part, 10);
                writestr_debug_serial("\n");
                break;
            case MULTIBOOT_TAG_TYPE_MMAP:
                {
                    writestr_debug_serial("INFO: FOUND TAG MULTIBOOT_TAG_TYPE_MMAP\n");
                    writestr_debug_serial(" mmap:\n");
                    multiboot_memory_map_t* mmap;
                    for(mmap = ((struct multiboot_tag_mmap*)tag)->entries;
                        (multiboot_uint8_t*)mmap < (multiboot_uint8_t*)tag + tag->size;
                        mmap = (multiboot_memory_map_t*)((unsigned long)mmap + ((struct multiboot_tag_mmap*)tag)->entry_size)
                    ){
                        writestr_debug_serial("  base_addr = 0x");
                        writeuint_debug_serial((unsigned) (mmap->addr >> 32), 16);
                        writeuint_debug_serial((unsigned) (mmap->addr & 0xffffffff), 16);
                        writestr_debug_serial(", length = 0x");
                        writeuint_debug_serial((unsigned) (mmap->len >> 32), 16);
                        writeuint_debug_serial((unsigned) (mmap->len & 0xffffffff), 16);
                        writestr_debug_serial(", type = 0x");
                        writeuint_debug_serial((unsigned) mmap->type);
                        writestr_debug_serial("\n");
                    }
                }
                break;
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                {
                    writestr_debug_serial("INFO: FOUND TAG MULTIBOOT_TAG_TYPE_FRAMEBUFFER\n");
                    unsigned i;
                    struct multiboot_tag_framebuffer* tagfb = (struct multiboot_tag_framebuffer*)tag;
                    writestr_debug_serial(" Framebuffer address: 0x");
                    writeuint_debug_serial((unsigned long)tagfb->common.framebuffer_addr, 16);
                    writestr_debug_serial("\n");
                    writestr_debug_serial(" Framebuffer type: 0x");
                    writeuint_debug_serial(tagfb->common.framebuffer_type, 16);
                    writestr_debug_serial("\n");
                    writestr_debug_serial(" Framebuffer width: ");
                    writeuint_debug_serial(tagfb->common.framebuffer_width, 10);
                    writestr_debug_serial("\n");
                    writestr_debug_serial(" Framebuffer height: ");
                    writeuint_debug_serial(tagfb->common.framebuffer_height, 10);
                    writestr_debug_serial("\n");
                    writestr_debug_serial(" Framebuffer BBP: ");
                    writeuint_debug_serial(tagfb->common.framebuffer_bpp, 10);
                    writestr_debug_serial("\n");
                    writestr_debug_serial(" Framebuffer red field position: ");
                    writeuint_debug_serial(tagfb->framebuffer_red_field_position, 10);
                    writestr_debug_serial("\n");
                    writestr_debug_serial(" Framebuffer green field position: ");
                    writeuint_debug_serial(tagfb->framebuffer_green_field_position, 10);
                    writestr_debug_serial("\n");
                    writestr_debug_serial(" Framebuffer blue field position: ");
                    writeuint_debug_serial(tagfb->framebuffer_blue_field_position, 10);
                    writestr_debug_serial("\n");
                    writestr_debug_serial(" Framebuffer pitch: ");
                    writeuint_debug_serial(tagfb->common.framebuffer_pitch, 10);
                    writestr_debug_serial("\n");


                    void* fb = (void*)(unsigned long)tagfb->common.framebuffer_addr;

                    /*
                        Very earlyt test draw code, to be moved out of this function later!!!
                        Once i make this func return a struct containing useful parsed info
                    */
                    if(tagfb->common.framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_RGB || tagfb->common.framebuffer_bpp != 32){
                        const char* oopsies = "AAAAAAAAAAAAAAAAAA";
                        multiboot_uint32_t* pixel = fb;
                        for(int i=0; i<16; i++){
                            *pixel = oopsies[i];
                            pixel = fb + i;
                        }
                        break; // Only doing this to test this RIGHT NOW! So dont care about anything that isnt RGB/32bit
                    }

                    multiboot_uint32_t colour = ((1 << tagfb->framebuffer_blue_mask_size) -1) << tagfb->framebuffer_blue_field_position;
                    
                    for(int x=0; x<800; x++){
                        for(int y=0; y<1280; y++){
                            multiboot_uint32_t* pixel = fb + (tagfb->common.framebuffer_pitch*x) + (4*y);
                            *pixel = colour;
                        }
                    }

                    break;
                }
            default:
                writestr_debug_serial("WARN: UNKNOWN TAG\n");
                break;
        }
        tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7));
    }

}