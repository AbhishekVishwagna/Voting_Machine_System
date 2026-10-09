/* timer.c - timer main file
 *
 * Created on: 13-Apr-2026
 * Author: Abhishek B Vishwagna
 *
 * Voting Machine Project
*/
#include "timer.h"

// FPGA INTERVAL TIMER 
#define TIMER_BASE     0xFF202000
#define TIMER_STATUS   (*(volatile unsigned int *)(TIMER_BASE + 0x00))
#define TIMER_CONTROL  (*(volatile unsigned int *)(TIMER_BASE + 0x04))
#define TIMER_PERIOD_L (*(volatile unsigned int *)(TIMER_BASE + 0x08))
#define TIMER_PERIOD_H (*(volatile unsigned int *)(TIMER_BASE + 0x0C))

// OUTPUT HARDWARE 
#define HEX_BASE 0xFF200020
#define LED_BASE 0xFF200000

static const unsigned char HEX_LUT[10] = {
    0x3F,0x06,0x5B,0x4F,0x66,
    0x6D,0x7D,0x07,0x7F,0x6F
};
#define DASH 0x40

// STATES 
typedef enum {
    ACTIVE,
    VOTED,
    EXPIRED
} VoterState;

static VoterState voterState = ACTIVE;

//  SYSTEM TIMER (10 minutes) 
static unsigned int system_time_remaining = 180;
static int systemClosed = 0;

//  VOTER TIMER (60 seconds) 
static unsigned int voter_time_remaining = 0;

// Called once when the machine boots
void SystemTimer_start(unsigned int seconds)
{
    system_time_remaining = seconds;
    systemClosed = 0;

    // Configure FPGA interval timer 
    unsigned int period = 50000000;
    TIMER_PERIOD_L = period & 0xFFFF;
    TIMER_PERIOD_H = period >> 16;
    TIMER_CONTROL  = 0x7;
}

// Called at the start of each voter
void VoterTimer_start(unsigned int seconds)
{
    voter_time_remaining = seconds;
    voterState = ACTIVE;
}

// Poll both system level and voter level timers
void Timer_poll(void)
{
    if (TIMER_STATUS & 0x1) {
        TIMER_STATUS = 0;

        //  SYSTEM‑LEVEL countdown 
        if (!systemClosed && system_time_remaining > 0) {
            system_time_remaining--;
            if (system_time_remaining == 0)
                systemClosed = 1;
        }

        //  VOTER‑LEVEL countdown 
        if (!systemClosed && voterState == ACTIVE && voter_time_remaining > 0) {
            voter_time_remaining--;
            if (voter_time_remaining == 0)
                voterState = EXPIRED;
        }
    }
}

void VoterTimer_voteConfirmed(void)
{
    if (!systemClosed && voterState == ACTIVE)
        voterState = VOTED;
}

void VoterTimer_reset(void)
{
    if (!systemClosed)
        VoterTimer_start(60);
}

// HEX display logic
void Timer_display(void)
{
    volatile unsigned char *HEX = (volatile unsigned char *)HEX_BASE;

    if (systemClosed) {
        HEX[0] = DASH;
        HEX[1] = DASH;
        return;
    }

    if (voterState == ACTIVE) {
        HEX[0] = HEX_LUT[voter_time_remaining % 10];
        HEX[1] = HEX_LUT[(voter_time_remaining / 10) % 10];
    }
    else if (voterState == VOTED) {
        HEX[0] = DASH;
        HEX[1] = DASH;
    }
    else {
        HEX[0] = HEX_LUT[0];
        HEX[1] = HEX_LUT[0];
    }
}

// LED logic
void Timer_updateLEDs(void)
{
    if (systemClosed)
        *(volatile unsigned int *)LED_BASE = 0xF;
    else if (voterState == ACTIVE)
        *(volatile unsigned int *)LED_BASE = 0x1;
    else if (voterState == VOTED)
        *(volatile unsigned int *)LED_BASE = 0x3;
    else
        *(volatile unsigned int *)LED_BASE = 0x2;
}

int System_isClosed(void)
{
    return systemClosed;
}
