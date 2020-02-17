/**
 * @file   audio_player.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @brief  Firmware for a talking robot toy.
 */

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "audio_player.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void delay(uint_least16_t delay_ms);

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void AudioPlayer_Init(struct audio_player *self_p)
{
    assert(self_p);

    *self_p = (__typeof__(*self_p)) {0};
}

bool AudioPlayer_IsPlaying(const struct audio_player *self_p)
{
    assert(self_p);
    return self_p->current_sound_p != NULL;
}

void AudioPlayer_Wait(const struct audio_player *self_p)
{
    assert(self_p);

    while (AudioPlayer_IsPlaying(self_p))
    {

    }
}

void AudioPlayer_Play(struct audio_player *self_p, const struct sound *sound_p)
{
    assert(self_p);
    assert(sound_p);

    self_p->current_sound_p = sound_p;
    self_p->current_position = 0;
}

void AudioPlayer_Stop(struct audio_player *self_p)
{
    assert(self_p);

    self_p->current_sound_p = NULL;
    self_p->current_position = 0;
}

bool AudioPlayer_HasMoreSamples(const struct audio_player *self_p)
{
    assert(self_p);

    return self_p->current_position < pgm_read_word(&(self_p->current_sound_p->size));
}

uint8_t AudioPlayer_GetNextSample(struct audio_player *self_p)
{
    assert(self_p);

    uint8_t sample = 0;
    if (AudioPlayer_HasMoreSamples(self_p))
    {
        sample = pgm_read_byte(&(self_p->current_sound_p->samples[self_p->current_position]));
        self_p->current_position += 1;
    }

    return sample;
}

void AudioPlayer_PlaySequence(struct audio_player *self_p, uint8_t *sequence_p, size_t length, uint_least16_t spacing)
{
    assert(self_p);
    assert(sequence_p);

    for (size_t i = 0; i < length; ++i)
    {
        const uint8_t id = sequence_p[i];

        if (AudioLibrary_IsValidId(id))
        {
            AudioPlayer_Play(self_p, AudioLibrary_GetSoundById(id));
            AudioPlayer_Wait(self_p);
            delay(spacing);
        }
        else if (id > AUDIOLIBRARY_MAX_ID)
        {
            delay(id - AUDIOLIBRARY_MAX_ID);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void delay(uint_least16_t delay_ms)
{
    uint_least16_t i = 0;
    while (i < delay_ms)
    {
        ++i;
        _delay_ms(1);
    }
}
