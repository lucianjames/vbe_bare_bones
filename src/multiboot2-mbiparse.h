#ifndef MULTIBOOT2_MBIPARSE_H
#define MULTIBOOT2_MBIPARSE_H

#include "multiboot2.h"
#include "debugserial.h"
/*
  We assume that init_debug_serial() has already been called,
  allowing us to send debug messages using the serial debug functions
*/
void parse_mb2i(unsigned long addr);

#endif