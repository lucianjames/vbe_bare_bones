#include "graphics.h"



/*
    Render a character to the screen at the given offset.
    zap-vga09.pdf shows the table of characters available, and their offsets (for setting char_idx)
*/
void draw_psf_char(struct MB2TAGS mb2is, int row_offset, int col_offset, int char_idx){
    /*
        Set up the pointers required to draw a character
    */
    psf1_header* header = (psf1_header*)&_binary_zap_vga09_psf_start;
    unsigned char* psf = (unsigned char*)&_binary_zap_vga09_psf_start + 4 + (char_idx*9);
    void* framebuffer = (void*)(unsigned long)mb2is.framebufinfo->common.framebuffer_addr;


    /*
        Column offset changes with different BPP values
    */
    int col_pixel_increment = 0;
    switch(mb2is.framebufinfo->common.framebuffer_bpp){
        case(32):
            col_pixel_increment = 4;
            break;
        case(24):
            col_pixel_increment = 3;
            break;
    }
    
    /*
        Create some very pretty colours
    */
    multiboot_uint32_t colour_white = 0b11111111111111111111111111111111 >> 32 - mb2is.framebufinfo->common.framebuffer_bpp;
    multiboot_uint32_t colour_black = 0x0;

    /*
        Draw the character!
        This isnt optimised very well.
    */
    for(int row=0; row<header->charsize; row++){
        for(int col=0; col<8; col++){
            multiboot_uint32_t* pixel = framebuffer + ((row+row_offset)*mb2is.framebufinfo->common.framebuffer_pitch) + (col+col_offset)*col_pixel_increment;
            *pixel = (*(psf+row) << col & 0b10000000)? colour_white : colour_black;
        }
    }
}


/*
    Draw a string char-by-char using draw_psf_char
*/
void draw_psf_str(struct MB2TAGS mb2is, int row_offset, int col_offset, const char* str){
    for(int i=0; str[i]!=0x00; i++){
        draw_psf_char(mb2is, row_offset, col_offset+(i*9), str[i]);
    }
}

/*
    Draw all the characters!
*/
void draw_psf_debug_matrix(struct MB2TAGS mb2is, int row_offset, int col_offset){
    unsigned char line[16];
    unsigned char current_char = 0x00;
    for(int i=0; i<16; i++){
        for(int j=0; j<16; j++){
            draw_psf_char(mb2is, row_offset+(i*10), col_offset+(j*10), current_char);
            current_char++;
        }
    }
}


/*

*/
void dump_psf_info(){
    writestr_debug_serial("INFO: Dumping PSF information...\n");
    writestr_debug_serial(" _binary_zap_vga09_psf_start: 0x");
    writeuint_debug_serial(&_binary_zap_vga09_psf_start, 16);
    writestr_debug_serial("\n _binary_zap_vga09_psf_end: 0x");
    writeuint_debug_serial(&_binary_zap_vga09_psf_end, 16);
    writestr_debug_serial("\n");
    psf1_header* psf = (psf1_header*)&_binary_zap_vga09_psf_start;
    if(psf->magic[0] == PSF1_MAGIC0 && psf->magic[1] == PSF1_MAGIC1){
        writestr_debug_serial(" PSF1 Magic: Valid (0x36, 0x04)\n");
    }else{
        writestr_debug_serial("ERROR: PSF1 magic NOT valid\n");
    }
    writestr_debug_serial(" PSF mode: 0x");
    writeuint_debug_serial(psf->mode, 16);
    writestr_debug_serial("\n PSF charsize: ");
    writeuint_debug_serial(psf->charsize, 10);
    writestr_debug_serial("\n");
}

void psf_test(struct MB2TAGS mb2is){
    dump_psf_info();

    while(1){
        draw_psf_str(mb2is, 0, 0, "Hello, world!");
    }
}

void flash_screen_graphics_test(struct MB2TAGS mb2is){
    multiboot_uint32_t colour_blue = ((1 << mb2is.framebufinfo->framebuffer_blue_mask_size) -1) << mb2is.framebufinfo->framebuffer_blue_field_position;
    multiboot_uint32_t colour_red = ((1 << mb2is.framebufinfo->framebuffer_red_mask_size) -1) << mb2is.framebufinfo->framebuffer_red_field_position;
    void* fb = (void*)(unsigned long)mb2is.framebufinfo->common.framebuffer_addr;
    size_t fb_size = mb2is.framebufinfo->common.framebuffer_height * mb2is.framebufinfo->common.framebuffer_pitch;
    void* fbc_max = fb+fb_size;
    switch(mb2is.framebufinfo->common.framebuffer_bpp){
        case(32):
        {
            #define FBC_INCREMENT 4
            while(1){
                void* fbc = fb;
                do{
                    fbc+=FBC_INCREMENT;
                    *(multiboot_uint32_t*)(fbc) = colour_blue;
                }while(fbc<fbc_max);
                fbc = fb;
                do{
                    fbc+=FBC_INCREMENT;
                    *(multiboot_uint32_t*)(fbc) = colour_red;
                }while(fbc<fbc_max);
            }
            break;
        }

        case(24):
        {   
            //colour_blue = (colour_blue & 0xffffff);
            #define FBC_INCREMENT 3
            while(1){
                void* fbc = fb;
                do{
                    fbc+=FBC_INCREMENT;
                    *(multiboot_uint32_t*)(fbc) = colour_blue;
                }while(fbc<fbc_max);
                fbc = fb;
                do{
                    fbc+=FBC_INCREMENT;
                    *(multiboot_uint32_t*)(fbc) = colour_red;
                }while(fbc<fbc_max);
            }
            break;
        }
    }
}