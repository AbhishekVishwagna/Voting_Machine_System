#ifndef TIMER_H
#define TIMER_H

// Start a voting session
void Timer_start(unsigned int seconds);

// Update timer logic (call repeatedly)
void Timer_poll(void);

// Show output on HEX display
void Timer_display(void);

// Update LED status
void Timer_updateLEDs(void);

// Register vote completion
void Timer_voteConfirmed(void);

// Reset system for next voter
void Timer_reset(void);

#endif
