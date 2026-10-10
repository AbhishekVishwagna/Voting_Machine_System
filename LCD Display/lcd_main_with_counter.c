/*
 * lcd_main_with_counter.c
 *
 *  Created on: 20 Apr 2026
 *      Author: Abhishek
 */
#include "FPGA_PIO/FPGA_PIO.h"
#include "DE1SoC_LT24/DE1SoC_LT24.h"
#include "DE1SoC_Addresses/DE1SoC_Addresses.h"
#include "graphics/graphics.h"

#include <stdlib.h>
#include <stdio.h>

//Error helper

static void exitOnFail(HpsErr_t s)
{
    if (ERR_IS_ERROR(s)) {
        exit((int)s);
    }
}

//Screen states
typedef enum {
    SCREEN_WELCOME,
    SCREEN_CANDIDATES,
    SCREEN_VOTED,
    SCREEN_RESULTS
} ScreenState_t;

//Vote counters
static unsigned int votes_party1 = 0;
static unsigned int votes_party2 = 0;
static unsigned int votes_nota   = 0;

//Inputs
#define SW9_MASK (1 << 9)

//Small delay for "VOTED" screen
static void shortDelay(void)
{
    volatile unsigned int i;
    for (i = 0; i < 400000; i++);
}

//Screen drawing functions
static void drawWelcome(LT24Ctx_t* lcd)
{
    LT24_clearDisplay(lcd, LT24_BLACK);
    gfx_drawStringScaled(lcd, 60, 90, "WELCOME", LT24_YELLOW, 3);
    gfx_drawStringScaled(lcd, 20, 160, "Press KEY0 to Vote", LT24_WHITE, 2);
}

static void drawCandidates(LT24Ctx_t* lcd)
{
    LT24_clearDisplay(lcd, LT24_BLACK);
    gfx_drawStringScaled(lcd, 30, 30, "Choose Candidate", LT24_WHITE, 2);
    gfx_drawStringScaled(lcd, 30, 90,  "ABC  - KEY1", LT24_RED, 2);
    gfx_drawStringScaled(lcd, 30, 140, "XYZ  - KEY2", LT24_GREEN, 2);
    gfx_drawStringScaled(lcd, 30, 190, "NOTA - KEY3", LT24_YELLOW, 2);
}

static void drawVoted(LT24Ctx_t* lcd)
{
    LT24_clearDisplay(lcd, LT24_BLACK);
    gfx_drawStringScaled(lcd, 90, 120, "VOTED", LT24_WHITE, 2);
}

static void drawResults(LT24Ctx_t* lcd)
{
    char buf[12];

    LT24_clearDisplay(lcd, LT24_BLACK);
    gfx_drawStringScaled(lcd, 70, 20, "RESULTS", LT24_WHITE, 2);

    gfx_drawStringScaled(lcd, 30, 80, "ABC:", LT24_RED, 2);
    sprintf(buf, "%u", votes_party1);
    gfx_drawStringScaled(lcd, 140, 80, buf, LT24_RED, 2);

    gfx_drawStringScaled(lcd, 30, 130, "XYZ:", LT24_GREEN, 2);
    sprintf(buf, "%u", votes_party2);
    gfx_drawStringScaled(lcd, 140, 130, buf, LT24_GREEN, 2);

    gfx_drawStringScaled(lcd, 30, 180, "NOTA:", LT24_YELLOW, 2);
    sprintf(buf, "%u", votes_nota);
    gfx_drawStringScaled(lcd, 140, 180, buf, LT24_YELLOW, 2);
}

//Main
int main(void)
{
    FPGAPIOCtx_t *gpio1 = NULL;
    FPGAPIOCtx_t *keys  = NULL;
    FPGAPIOCtx_t *sw    = NULL;
    LT24Ctx_t* lcd      = NULL;

    unsigned int keyState, prevKeyState;
    unsigned int swState;

    ScreenState_t screen = SCREEN_WELCOME;

    /* Initialise hardware */
    exitOnFail(FPGA_PIO_initialise(LSC_BASE_GPIO_JP1, LSC_CONFIG_GPIO, &gpio1));
    exitOnFail(LT24_initialise(&gpio1->gpio, NULL, &lcd));
    exitOnFail(FPGA_PIO_initialise(LSC_BASE_KEYS, LSC_CONFIG_KEYS, &keys));
    exitOnFail(FPGA_PIO_initialise(LSC_BASE_SLIDE_SWITCH, LSC_CONFIG_SLIDE_SWITCH, &sw));

    FPGA_PIO_getInput(keys, &keyState, LSC_KEYS_MASK);
    prevKeyState = keyState;

    drawWelcome(lcd);

    //Main loop
    while (1)
    {
        FPGA_PIO_getInput(keys, &keyState, LSC_KEYS_MASK);
        FPGA_PIO_getInput(sw, &swState, LSC_SLIDE_SWITCH_MASK);

        switch (screen)
        {
            case SCREEN_WELCOME:
                if (swState & SW9_MASK) {
                    drawResults(lcd);
                    screen = SCREEN_RESULTS;
                }
                else if ((prevKeyState & 0x1) && !(keyState & 0x1)) {
                    drawCandidates(lcd);
                    screen = SCREEN_CANDIDATES;
                }
                break;

            case SCREEN_CANDIDATES:
                if (swState & SW9_MASK) {
                    drawResults(lcd);
                    screen = SCREEN_RESULTS;
                }
                else if ((prevKeyState & 0x2) && !(keyState & 0x2)) {
                    votes_party1++;
                    drawVoted(lcd);
                    shortDelay();
                    drawWelcome(lcd);
                    screen = SCREEN_WELCOME;
                }
                else if ((prevKeyState & 0x4) && !(keyState & 0x4)) {
                    votes_party2++;
                    drawVoted(lcd);
                    shortDelay();
                    drawWelcome(lcd);
                    screen = SCREEN_WELCOME;
                }
                else if ((prevKeyState & 0x8) && !(keyState & 0x8)) {
                    votes_nota++;
                    drawVoted(lcd);
                    shortDelay();
                    drawWelcome(lcd);
                    screen = SCREEN_WELCOME;
                }
                break;

            case SCREEN_RESULTS:
                /* Stay here while SW9 is held */
                if (!(swState & SW9_MASK)) {
                    drawWelcome(lcd);
                    screen = SCREEN_WELCOME;
                }
                break;

            default:
                break;
        }

        prevKeyState = keyState;
    }
}
