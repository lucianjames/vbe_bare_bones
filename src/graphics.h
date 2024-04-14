#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "multiboot2-mbiparse.h" // For custom mbi info struct + includes multiboot2.h
#include "debugserial.h"


#define PSF1_MAGIC0     0x36
#define PSF1_MAGIC1     0x04

#define PSF1_MODE512    0x01
#define PSF1_MODEHASTAB 0x02
#define PSF1_MODEHASSEQ 0x04
#define PSF1_MAXMODE    0x05

#define PSF1_SEPARATOR  0xFFFF
#define PSF1_STARTSEQ   0xFFFE

typedef struct {
        unsigned char magic[2];     /* Magic number */
        unsigned char mode;         /* PSF font mode */
        unsigned char charsize;     /* Character size */
} psf1_header;


extern char _binary_zap_vga09_psf_start;
extern char _binary_zap_vga09_psf_end;

void draw_psf_char(struct MB2TAGS mb2is, int row_offset, int col_offset, int char_idx);
void draw_psf_str(struct MB2TAGS mb2is, int row_offset, int col_offset, const char* str);
void draw_psf_debug_matrix(struct MB2TAGS mb2is, int row_offset, int col_offset);
void dump_psf_info();
void psf_test(struct MB2TAGS mbi2_info_struct);
void flash_screen_graphics_test(struct MB2TAGS mbi2_info_struct);

#endif