/*
 * lcd_main.c
 *
 *  Created on: 20 Apr 2026
 *      Author: Abhishek
 */

#include "FPGA_PIO/FPGA_PIO.h"
#include "DE1SoC_LT24/DE1SoC_LT24.h"
#include "DE1SoC_Addresses/DE1SoC_Addresses.h"
#include "Util/watchdog.h"
#include "graphics/graphics.h"

#include <stdlib.h>

/*Helper to stop on error*/

static void exitOnFail(HpsErr_t s)
{
    if (ERR_IS_ERROR(s)) {
        exit((int)s);
    }
}

/*Screen state */
typedef enum {
    SCREEN_MENU,
    SCREEN_VOTE_CONFIRM
} ScreenState_t;

/* Draw menu screen*/

static void drawMenu(LT24Ctx_t* lcd)
{
    LT24_clearDisplay(lcd, LT24_BLACK);

    gfx_drawStringScaled(lcd, 20, 20,  "Party1 - KEY 0",    LT24_RED,    2);
    gfx_drawStringScaled(lcd, 20, 70,  "Party2 - KEY 1", LT24_WHITE,  2);
    gfx_drawStringScaled(lcd, 20, 120, "Party3 - KEY 2",    LT24_GREEN,  2);
    gfx_drawStringScaled(lcd, 20, 170, "NOTA - KEY 3",   LT24_YELLOW, 2);
}

/* Main*/
int main(void)
{
    FPGAPIOCtx_t* gpio1 = NULL;
    FPGAPIOCtx_t* keys  = NULL;
    LT24Ctx_t* lcd      = NULL;

    unsigned int keyState;
    unsigned int prevKeyState = 0xFFFFFFFF;

    ScreenState_t screen = SCREEN_MENU;
    unsigned int screenTimer = 0;

    /* Initialise GPIO (JP1, used by LCD) */
    exitOnFail(FPGA_PIO_initialise(
        LSC_BASE_GPIO_JP1,
        LSC_CONFIG_GPIO,
        &gpio1));

    /* Initialise LT24 LCD */
    exitOnFail(LT24_initialise(&gpio1->gpio, NULL, &lcd));

    /* Initialise push buttons */
    exitOnFail(FPGA_PIO_initialise(
        LSC_BASE_KEYS,
        LSC_CONFIG_KEYS,
        &keys));

    /* Initial screen */
    drawMenu(lcd);

    /*Main loop*/
    while (1)
    {
        ResetWDT();

        FPGA_PIO_getInput(keys, &keyState, 0xFFFFFFFF);

        /*MENU SCREEN*/
        if (screen == SCREEN_MENU)
        {
            /* KEY0 pressed */
            if ((prevKeyState & 0x1) && !(keyState & 0x1)) {
                LT24_clearDisplay(lcd, LT24_BLACK);
                gfx_drawStringScaled(lcd, 20, 100,
                    "VOTE REGISTERED", LT24_RED, 2);
                screen = SCREEN_VOTE_CONFIRM;
                screenTimer = 0;
            }
            /* KEY1 pressed */
            else if ((prevKeyState & 0x2) && !(keyState & 0x2)) {
                LT24_clearDisplay(lcd, LT24_BLACK);
                gfx_drawStringScaled(lcd, 20, 100,
                    "VOTE REGISTERED", LT24_WHITE, 2);
                screen = SCREEN_VOTE_CONFIRM;
                screenTimer = 0;
            }
            /* KEY2 pressed */
            else if ((prevKeyState & 0x4) && !(keyState & 0x4)) {
                LT24_clearDisplay(lcd, LT24_BLACK);
                gfx_drawStringScaled(lcd, 20, 100,
                    "VOTE REGISTERED", LT24_GREEN, 2);
                screen = SCREEN_VOTE_CONFIRM;
                screenTimer = 0;
            }
        }

        /*VOTE CONFIRM SCREEN*/
        else if (screen == SCREEN_VOTE_CONFIRM)
        {
            screenTimer++;

            /* After some time, go back to menu */
            if (screenTimer > 1500000) {   // To adjust timing if needed
                drawMenu(lcd);
                screen = SCREEN_MENU;
            }
        }

        prevKeyState = keyState;
    }
}

