/* audio.c — Audio Module Implementation
 *
 * Created on: 16-Apr-2026
 * Author: Abhishek
 *
 * Voting Machine Project
 */

#include "audio.h"
#include "Util/watchdog.h"
#include "Util/macros.h"
#include "DE1SoC_Addresses/DE1SoC_Addresses.h"
#include "DE1SoC_WM8731/DE1SoC_WM8731.h"
#include "HPS_GPIO/HPS_GPIO.h"
#include "HPS_I2C/HPS_I2C.h"

#include <math.h>
#include <stdlib.h>

static void exitOnFail(HpsErr_t status) {
    if (ERR_IS_ERROR(status)) {
        exit((int)status);
    }
}

/* Driver handles */
static WM8731Ctx_t* audio  = NULL; // WM8731 audio codec handle
static HPSI2CCtx_t* hpsi2c = NULL; // I2C bus handle
static HPSGPIOCtx_t* gpio  = NULL; // GPIO handle

/*
 * play_tone()
 * Generates a sine wave tone at the given frequency.
 */
static void play_tone(double freq_hz, unsigned int num_samples) {
    unsigned int sampleRate;
    WM8731_getSampleRate(audio, &sampleRate);  // Sample rate.


    double inc   = freq_hz * M_2PI / sampleRate; // phase increment per sample.
    double ampl  = 1073741824.0;  // amplitude = 2^30, maximum safe volume level.
    double phase = 0.0;           // phase accumulator.
    unsigned int i;

    for (i = 0; i < num_samples; i++) {
        unsigned int space;
        WM8731_getFIFOSpace(audio, &space); // check how many slots are free.
 
        /* wrap phase back to 0-2Pi range to keep sin() accurate. */
        if (space > 0) {
            phase += inc;
            while (phase >= M_2PI) {
                phase -= M_2PI;
            }
        
            /* calculate the next sine wave sample*/
            signed int sample = (signed int)(ampl * sin(phase));
            WM8731_writeSample(audio, sample, sample);
        }

        ResetWDT();  // Reset Watchdog
    }
}

/* 
 * audio_init()
 * Initialises GPIO, I2C and WM8731 audio codec.
 * Must be called once at the start of main().
 */
void audio_init(void) {
    exitOnFail(HPS_GPIO_initialise(LSC_BASE_ARM_GPIO, ARM_GPIO_DIR,
                                   ARM_GPIO_I2C_GENERAL_MUX, 0, &gpio));
    exitOnFail(HPS_I2C_initialise(LSC_BASE_I2C_GENERAL,
                                   I2C_SPEED_STANDARD, &hpsi2c));
    exitOnFail(WM8731_initialise(LSC_BASE_AUDIOCODEC,
                                  &hpsi2c->i2c, &audio));
    WM8731_clearFIFO(audio, true, true);
}

/*
 * play_vote_beep()
 * Loud 950 Hz beep.
 * Call each time a vote is successfully casted.
*/
void play_vote_beep(void) {
    play_tone(950.0, 20000);
}

/*
 * play_close_fanfare()
 * Rising 3-tone fanfare using musical notes C5, E5 and G5.
 * Call once when the election closes.
 */
void play_close_fanfare(void) {
    play_tone(523.25, 19200);     // C5 low  tone.
    play_tone(659.25, 19200);     //E5 mid  tone.
    play_tone(783.99, 38400);     // G5 high tone, held longer for effect.
}
