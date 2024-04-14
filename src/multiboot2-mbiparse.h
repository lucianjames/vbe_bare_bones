#ifndef MULTIBOOT2_MBIPARSE_H
#define MULTIBOOT2_MBIPARSE_H

#include "multiboot2.h"
#include "debugserial.h"

struct MB2TAGS {
    struct multiboot_tag_framebuffer* framebufinfo;
    // More will go here when needed!
};

/*
  We assume that init_debug_serial() has already been called,
  allowing us to send debug messages using the serial debug functions
*/
struct MB2TAGS get_mb2i_tags(unsigned long addr);

#endif