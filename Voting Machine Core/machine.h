/* game.h - ELEC5620 Mini project : voting machine definitions header file
 *
 * Created on: 23-Apr-2026
 * Author: Abhishek B Vishwagna
 *
 *All team memebers must put this file in #include
*/

#ifndef MACHINE_H_
#define MACHINE_H_

/* Base addresses of all the modules used */
#define KEY_BASE       0xFF200050  /* Push buttons */
#define LED_BASE       0xFF200000  /* Red LEDs */
#define HEX_BASE       0xFF200020  /* 7 segement display */
#define TIMER_BASE     0xFF202000  /* Interval timer */
#define SW_BASE        0xFF200040   /* Slide switches SW0-SW9   */

/*  Voting machine configuration */
#define NUM_OF_CANDIDATES   3    /* Number of candiates */
#define VOTING_WINDOW     600     /* Timer for the whole elction window */
#define VOTER_TIME         60    /* 60 seconds per individual voter */
#define STATUS_NO_VOTES_CAST  -1 /* when no vote is casted */
#define STATUS_ELECTION_TIE   -2 /* When there is a tie between 2 candidates */

/*  Volatile hardware pointers */
#define KEY_ptr     ((volatile int *) KEY_BASE)
#define LED_ptr     ((volatile int *) LED_BASE)
#define TIMER_ptr   ((volatile int *) TIMER_BASE)
#define SW_ptr      ((volatile int *) SW_BASE) 

/*  Voting machine driver contents */
typedef struct {
    Candidate candidates[NUM_OF_CANDIDATES];
    int num_of_candidates;
    int window_remaining;   
    int voter_remaining;    
    int total_votes;
    GameState state;
    int voter_active;
} VotingSystem;

/* voting machine states */
typedef enum {
    STATE_IDLE      = 0,
    STATE_VOTING    = 1,
    STATE_CLOSED    = 2,
    STATE_RESULT    = 3
} GameState;

/* Candidate Data */
typedef struct {
    char name[20];
    int votes;
} Candidate;

/* Function Declarations (defined in their .c files respectively) */

void vs_init(VotingSystem *vs);
void vs_add_vote(VotingSystem *vs, int candidate_index);
int vs_get_winner(VotingSystem *vs);
void vs_reset(VotingSystem *vs);


#endif /* MACHINE_H_ */
