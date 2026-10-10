#ifndef GRAPHICS_GRAPHICS_H_
#define GRAPHICS_GRAPHICS_H_

#include "DE1SoC_LT24/DE1SoC_LT24.h"

/* Draw 1 pixel */
void gfx_drawPixel(
    LT24Ctx_t* lcd,
    int x,
    int y,
    unsigned short colour);

/* Draw 1 ASCII character (8×8) */
void gfx_drawChar(
    LT24Ctx_t* lcd,
    int x,
    int y,
    char c,
    unsigned short colour);

/* Draw a string */
void gfx_drawString(
    LT24Ctx_t* lcd,
    int x,
    int y,
    const char* str,
    unsigned short colour);

/* Draw filled rectangle */
void gfx_fillRect(
    LT24Ctx_t* lcd,
    int x,
    int y,
    int w,
    int h,
    unsigned short colour);

/* Draw scaled character */
void gfx_drawCharScaled(
    LT24Ctx_t* lcd,
    int x,
    int y,
    char c,
    unsigned short colour,
    int scale);

/* Draw scaled string */
void gfx_drawStringScaled(
    LT24Ctx_t* lcd,
    int x,
    int y,
    const char* str,
    unsigned short colour,
    int scale);

#endif /* GRAPHICS_GRAPHICS_H_ */
