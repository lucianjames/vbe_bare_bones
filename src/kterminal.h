#ifndef KTERMINAL_H
#define KTERMINAL_H

#include <stdarg.h>

#include "multiboot2-mbiparse.h"
#include "graphics.h"

int G_KTERM_CROW = 0;
int G_KTERM_MAXROW = 0;
int G_KTERM_MAXCOL = 0;
struct MB2TAGS G_KTERM_MB2_IS;

void kterm_init(struct MB2TAGS mb2is);
int kterm_printuint(int col, uint32_t uint_to_write, int base);
void kterm_printf_newline(const char* fmt, ...);
void kterm_write_newline(const char* str);
void kterm_clear();

#endif