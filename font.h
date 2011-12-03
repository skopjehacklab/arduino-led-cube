#ifndef FONT_H
#define FONT_H

#include <avr/pgmspace.h>

void font_getchar (char chr, unsigned char dst[5]);
void font_getpath (unsigned char path, unsigned char *destination, int length);
void font_getbitmap (char bitmap, unsigned char dst[8]);
unsigned char font_getbitmappixel ( char bitmap, char x, char y);



#endif
