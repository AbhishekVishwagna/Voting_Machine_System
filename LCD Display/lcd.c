/*
 * lcd.c
 *
 *  Created on: 20 Apr 2026
 *      Author: Abhishek
 */
#include "lcd.h"
#include "DE1SoC_Addresses/DE1SoC_Addresses.h"
#include "graphics/graphics.h"

#include <stdlib.h>
#include <stdio.h>

static FPGAPIOCtx_t *gpio1 = NULL;
static LT24Ctx_t* lcd      = NULL;

static void exitOnFail(HpsErr_t s)
{
    if (ERR_IS_ERROR(s)) {
        exit((int)s);
    }
}

static void lcdDelay(void)
{
    volatile unsigned int i;
    for (i = 0; i < 30000; i++);
}

//PUBLIC FUNCTIONS
void lcd_init(void)
{
    exitOnFail(FPGA_PIO_initialise(LSC_BASE_GPIO_JP1, LSC_CONFIG_GPIO, &gpio1));
    exitOnFail(LT24_initialise(&gpio1->gpio, NULL, &lcd));

    LT24_clearDisplay(lcd, LT24_BLACK);
    lcdDelay();
}

//SCREEN FUNCTIONS

void lcd_showWelcome(void) // Intial screen to start with welcome.
{
    LT24_clearDisplay(lcd, LT24_BLACK);
    lcdDelay();

    gfx_drawStringScaled(lcd, 40, 120, "WELCOME", LT24_YELLOW, 3);
}

void lcd_showCandidates(void) // Shows candidate and the key details.
{
    LT24_clearDisplay(lcd, LT24_BLACK);
    lcdDelay();

    gfx_drawStringScaled(lcd, 20, 80,  "ABC  - KEY1", LT24_RED, 2);
    gfx_drawStringScaled(lcd, 20, 140, "XYZ  - KEY2", LT24_GREEN, 2);
    gfx_drawStringScaled(lcd, 20, 200, "NOTA - KEY3", LT24_YELLOW, 2);
}
void lcd_showVoted(void) // To show "Voted" after the user presses the voting button
{
    LT24_clearDisplay(lcd, LT24_BLACK);
    lcdDelay();
    gfx_drawStringScaled(lcd,60, 120,"VOTED",LT24_WHITE,3);
}

void lcd_showResults(unsigned int p1,unsigned int p2,unsigned int nota)
{
    char buf_votes[10]; // Votes
    char buf_perc[10]; // Vote Percentage

    unsigned int total_votes = p1 + p2 + nota;

    unsigned int p1_perc = 0;
    unsigned int p2_perc = 0;
    unsigned int nota_perc = 0;

    if (total_votes > 0) {
        p1_perc   = (p1   * 100) / total_votes;
        p2_perc   = (p2   * 100) / total_votes;
        nota_perc = (nota * 100) / total_votes;
    }

    LT24_clearDisplay(lcd, LT24_BLACK);
    lcdDelay();

    //Title
    gfx_drawStringScaled(lcd, 70, 20,"RESULTS", LT24_WHITE, 2);

    //COLUMN HEADER
    gfx_drawStringScaled(lcd, 20, 60,"PARTY",LT24_WHITE, 1);
    gfx_drawStringScaled(lcd, 120, 60,"VOTES",LT24_WHITE, 1);
    gfx_drawStringScaled(lcd, 190, 60,"PERC",LT24_WHITE, 1);

    //Party ABC
    sprintf(buf_votes, "%u", p1); // To display the Votes
    sprintf(buf_perc,  "%u%%", p1_perc); // To display the Vote precentage with "%"
    
    gfx_drawStringScaled(lcd, 20, 100, "ABC", LT24_RED, 2);
    gfx_drawStringScaled(lcd, 120, 100, buf_votes, LT24_RED, 2);
    gfx_drawStringScaled(lcd, 190,100, buf_perc, LT24_RED, 2);

    // Party XYZ
    sprintf(buf_votes, "%u", p2);
    sprintf(buf_perc,  "%u%%", p2_perc);

    gfx_drawStringScaled(lcd, 20, 150, "XYZ", LT24_GREEN, 2);
    gfx_drawStringScaled(lcd, 120, 150, buf_votes, LT24_GREEN, 2);
    gfx_drawStringScaled(lcd, 190,150, buf_perc, LT24_GREEN, 2);

    // NOTA
    sprintf(buf_votes, "%u", nota);
    sprintf(buf_perc,  "%u%%", nota_perc);

    gfx_drawStringScaled(lcd, 20, 200, "NOTA", LT24_YELLOW, 2);
    gfx_drawStringScaled(lcd, 120, 200, buf_votes, LT24_YELLOW, 2);
    gfx_drawStringScaled(lcd, 190,200, buf_perc, LT24_YELLOW, 2);
}
void lcd_showWinner(unsigned int p1,unsigned int p2,unsigned int nota)
{
    uint16_t bgColor = LT24_WHITE;
    const char *winnerText = "NO VOTES";

    LT24_clearDisplay(lcd, LT24_BLACK);
    lcdDelay();

    // Determine winner
    if (p1 == 0 && p2 == 0 && nota == 0) {
        bgColor = LT24_YELLOW;
        winnerText = "NO VOTES";
    }
    else if ((p1 == p2 && p1 >= nota) ||(p1 == nota && p1 >= p2) ||(p2 == nota && p2 >= p1)) {
        bgColor = LT24_YELLOW;
        winnerText = "TIE";
    }
    else if (p1 > p2 && p1 > nota) {
        bgColor = LT24_RED;
        winnerText = "ABC";
    }
    else if (p2 > p1 && p2 > nota) {
        bgColor = LT24_GREEN;
        winnerText = "XYZ";
    }
    else {
        bgColor = LT24_YELLOW;
        winnerText = "NOTA";
    }

    //Draw title
    gfx_drawStringScaled(lcd, 60, 30,"WINNER",LT24_WHITE, 2);

    //Draw colored banner
    gfx_fillRect(lcd,20, 100,280, 80, bgColor);

    //Draw winner name on banner
    gfx_drawStringScaled(lcd,90, 120,winnerText,LT24_BLACK, 4);
}