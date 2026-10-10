/*
 * graphics.h
 *
 *  Created on: 20 Apr 2026
 *      Author: Abhishek
 */
#include "graphics.h"
#include "lcd_display.h"

/* Draw a single pixel */
void gfx_drawPixel(LT24Ctx_t* lcd, int x, int y, unsigned short colour)
{
    LT24_drawPixel(lcd, colour, x, y);
}

/* Draw a single 8×8 character */
void gfx_drawChar(LT24Ctx_t* lcd, int x, int y, char c, unsigned short colour)
{
    int row, col;
    unsigned char bits;

    if (c < 0 || c > 127) return;

    for (row = 0; row < 8; row++) {
        bits = font8x8_basic[(int)c][row];
        for (col = 0; col < 8; col++) {
            if (bits & (1 << (7 - col))) {
                gfx_drawPixel(lcd, x + col, y + row, colour);
            }
        }
    }
}

/* Draw string */
void gfx_drawString(LT24Ctx_t* lcd, int x, int y,
                    const char* str, unsigned short colour)
{
    while (*str) {
        gfx_drawChar(lcd, x, y, *str, colour);
        x += 8;
        str++;
    }
}

/* Filled rectangle */
void gfx_fillRect(LT24Ctx_t* lcd, int x, int y,
                  int w, int h, unsigned short colour)
{
    int i, j;

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            gfx_drawPixel(lcd, x + i, y + j, colour);
        }
    }
}

/* Draw scaled character */
void gfx_drawCharScaled(
    LT24Ctx_t* lcd,
    int x,
    int y,
    char c,
    unsigned short colour,
    int scale)
{
    int row, col, dx, dy;
    unsigned char bits;

    if (c < 0 || c > 127) return;

    for (row = 0; row < 8; row++) {
        bits = font8x8_basic[(int)c][row];
        for (col = 0; col < 8; col++) {
            if (bits & (1 << (7 - col))) {
                for (dy = 0; dy < scale; dy++) {
                    for (dx = 0; dx < scale; dx++) {
                        gfx_drawPixel(
                            lcd,
                            x + col * scale + dx,
                            y + row * scale + dy,
                            colour);
                    }
                }
            }
        }
    }
}

/* Draw scaled string */
void gfx_drawStringScaled(
    LT24Ctx_t* lcd,
    int x,
    int y,
    const char* str,
    unsigned short colour,
    int scale)
{
    while (*str) {
        gfx_drawCharScaled(lcd, x, y, *str, colour, scale);
        x += 8 * scale;
        str++;
    }
}
