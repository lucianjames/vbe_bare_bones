#ifndef MULTIBOOT2_MBIPARSE_H
#define MULTIBOOT2_MBIPARSE_H

#include "multiboot2.h"
#include "debugserial.h"

struct MBI2_INFO{
    struct multiboot_tag_framebuffer* framebufinfo
    // More will go here when needed!
};

/*
  We assume that init_debug_serial() has already been called,
  allowing us to send debug messages using the serial debug functions
*/
struct MBI2_INFO parse_mb2i(unsigned long addr);

#endif