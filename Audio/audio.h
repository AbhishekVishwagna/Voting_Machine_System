#ifndef AUDIO_H
#define AUDIO_H

/* Initialise the WM8731 audio codec */
void audio_init(void);

/* Play a loud beep when a vote is casted */
void play_vote_beep(void);

/* Play a rising 3-tone fanfare when the election closes */
void play_close_fanfare(void);

#endif /* AUDIO_H */
