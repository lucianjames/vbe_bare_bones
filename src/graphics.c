#include "graphics.h"

void flash_screen_graphics_test(struct MBI2_INFO mbi2_info_struct){
    multiboot_uint32_t colour_blue = ((1 << mbi2_info_struct.framebufinfo->framebuffer_blue_mask_size) -1) << mbi2_info_struct.framebufinfo->framebuffer_blue_field_position;
    multiboot_uint32_t colour_red = ((1 << mbi2_info_struct.framebufinfo->framebuffer_red_mask_size) -1) << mbi2_info_struct.framebufinfo->framebuffer_red_field_position;
    void* fb = (void*)(unsigned long)mbi2_info_struct.framebufinfo->common.framebuffer_addr;
    size_t fb_size = mbi2_info_struct.framebufinfo->common.framebuffer_height * mbi2_info_struct.framebufinfo->common.framebuffer_pitch;
    switch(mbi2_info_struct.framebufinfo->common.framebuffer_bpp){
        case(32):
        {
            while(1){
                void* fbc = fb;
                void* fbc_max = fb+(fb_size);
                do{
                    fbc+=4;
                    *(multiboot_uint32_t*)(fbc) = colour_blue;
                }while(fbc<fbc_max);
                fbc = fb;
                do{
                    fbc+=4;
                    *(multiboot_uint32_t*)(fbc) = colour_red;
                }while(fbc<fbc_max);
            }
            break;
        }

        case(24):
        {   
            //colour_blue = (colour_blue & 0xffffff);
            while(1){
                void* fbc = fb;
                void* fbc_max = fb+(fb_size);
                do{
                    fbc+=3;
                    *(multiboot_uint32_t*)(fbc) = colour_blue;
                }while(fbc<fbc_max);
                fbc = fb;
                do{
                    fbc+=3;
                    *(multiboot_uint32_t*)(fbc) = colour_red;
                }while(fbc<fbc_max);
            }
            break;
        }
    }
}