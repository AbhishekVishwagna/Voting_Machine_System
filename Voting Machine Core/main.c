/* main.c - Voting machine main file
 *
 * Created on: 23-Apr-2026
 * Author: Abhishek B Vishwagna
 *
 * ELEC5620M Mini project
*/

#include "machine.h"
#include "string.h"
#include "Util/watchdog.h"
#include <stdio.h> 

/* voting machine initalisation */
void vs_init(VotingSystem *vs)
{
    vs->num_of_candidates = NUM_OF_CANDIDATES;
    vs->state             = STATE_IDLE;
    vs->total_votes       = 0;
    vs->window_remaining  = VOTING_WINDOW;
    vs->voter_remaining   = 0;
    vs->voter_active      = 0;

    /* Candidate names changed to match scenario */
    strncpy(vs->candidates[0].name, "Candiate A", 20);
    strncpy(vs->candidates[1].name, "Candiate B", 20);
    strncpy(vs->candidates[2].name, "Candiate C", 20);

    /* Initialize all candidate vote counts to zero */
    for (int i=0;i<NUM_OF_CANDIDATES;i++)
    {
        vs->candidates[i].votes = 0;
    }

}   

/* To increments the vote count for a specific candidate when the voter casts the vote*/
void vs_add_vote(VotingSystem *vs, int candidate_index) {

    if (vs->state != STATE_VOTING)      // To ensure the voting machine state is open for voting
    {
        return;
    }

    if (candidate_index < 0 || candidate_index >= vs->num_of_candidates)    // To validate candidate index to prevent memory overflow
    {
        return;
    }

    //process the votes
    vs->candidates[candidate_index].votes++;
    vs->total_votes++;
}

/* Checks the current vote counts to determine the winner or a tie*/
int vs_get_winner(VotingSystem *vs) {

    //Firstly ensuring we have something to count
    if (vs->total_votes == 0) {
        return STATUS_NO_VOTES_CAST;;
    }

    int max_votes = -1;
    int winner_index = -1;
    int tie = 0;

    for (int i=0;i < vs->num_of_candidates; i++) {
        int current_votes = vs->candidates[i].votes;

        //The New leader condition
        if(current_votes > max_votes) {
            max_votes = current_votes;
            winner_index = i;
            tie = 0;
        }

        // if we have stalemate condition
        else if (current_votes == max_votes) {
            tie = 1;
        }
    }

    // To relsove the tie condition
    if (tie) {
        return STATUS_ELECTION_TIE;
    }

    return winner_index;
    
}

/* Resets the voting system  */
void vs_reset(VotingSystem *vs) {
    vs_init(vs);   // Re initalise Everything
}


/* Main Program */
int main(void)
{
    ResetWDT();
    VotingSystem vs;
    vs_init(&vs);    /* Initalise Voting machine  */

    int prev_keys = 0;
    unsigned int switch_values = *SW_ptr;
    while(1)
    {
        ResetWDT();    // Continuously reset watchdog in the main loop
        
        /* The button debounce logic */
        int current_keys = *KEY_ptr & 0xF;
        int just_pressed = current_keys & ~prev_keys;
        prev_keys = current_keys;

        /* the machine logic */
        switch (vs.state)
        {

        case STATE_IDLE:
            /* Start the election if SW0 is toggled ON   */
            if(switch_values & 0x1)
            {
                vs.state            = STATE_VOTING;
                vs.window_remaining = VOTING_WINDOW;
                vs.voter_active     = 0;
                results_printed     = 0;
                // To be done by anirudh start voting timer for 10 minute window
                // to be done by Santosh welcome screen
            }
            break;

        case STATE_VOTING:

            if (vs.voter_active == 0)
            {
                    // To be done by santosh ,show voting screen

                    /* KEY0 pressed to give the voter 60 seconds */
                    if (just_pressed & 0x1)
                    {
                        vs.voter_active   = 1;
                        vs.voter_remaining = VOTER_TIME;
                        // To be done by anirudh , update timer for the 60 seconds
                        // To be done by santosh ,show candidates screen
                    }
                }
            else
                {
                    /* Check if voter's 60 seconds expired with no vote */
                    if (vs.voter_remaining <= 0)
                    {
                        vs.voter_active = 0;   /* time up — no vote */
                        //Voter time expired. No vote registered
                        break;
                    }

                    /* KEY1 pressed — vote for Candidate A */
                    if (just_pressed & 0x2)
                    {
                        vs_add_vote(&vs, 0);
                        vs.voter_active = 0;   /* stop voter timer */
                        // to done by arfat , voter beep
                    }

                    /* KEY2 pressed — vote for Candidate B */
                    if (just_pressed & 0x4)
                    {
                        vs_add_vote(&vs, 1);
                        vs.voter_active = 0;
                        // to done by arfat , voter beep
                    }

                    /* KEY3 pressed — vote for Candidate C */
                    if (just_pressed & 0x8)
                    {
                        vs_add_vote(&vs, 2);
                        vs.voter_active = 0;
                        // to done by arfat , voter beep
                    }
                }
                break;

        case STATE_CLOSED:
            /* The transition to result processing*/
            //to be done by arfat, play turn fanfare for voting system
            vs.state =STATE_RESULT;
            break;

        case STATE_RESULT:
            /* Display final results once  */
            // To be done by anirudh , update timer for the 60 seconds
            // To be done by santosh ,show candidates screen
            if (!(switch_values & 0x1))
                {
                    vs_reset(&vs);
                    //to be done by santosh, show the result screen
                }
                break;

        default:
            vs_reset(&vs);  // the voting machine goes to intialisation function
            break;
        }
        // To be done by anirudh , update the 7 segement display and the LEDs
    }
}