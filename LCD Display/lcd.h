#ifndef LCD_H
#define LCD_H

#include "FPGA_PIO/FPGA_PIO.h"
#include "DE1SoC_LT24/DE1SoC_LT24.h"

// Initialisation
void lcd_init(void);

// Four different Screens
void lcd_showWelcome(void); // Welcome screen
void lcd_showCandidates(void); // Display candidate Names
void lcd_showVoted(void); // Display "Voted"
void lcd_showWinner(unsigned int p1,unsigned int p2,unsigned int nota); // Display winner
void lcd_showResults(unsigned int p1, unsigned int p2, unsigned int nota); // Display the Vote shares

#endif
