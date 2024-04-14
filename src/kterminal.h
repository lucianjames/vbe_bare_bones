#ifndef KTERMINAL_H
#define KTERMINAL_H

#include <stdarg.h>

#include "multiboot2-mbiparse.h"
#include "graphics.h"

void kterm_init(struct MB2TAGS mb2is);
int kterm_printuint(int col, uint32_t uint_to_write, int base);
void kterm_printf_newline(const char* fmt, ...);
void kterm_write_newline(const char* str);
void kterm_clear();

#endif