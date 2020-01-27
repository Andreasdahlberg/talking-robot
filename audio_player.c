
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "audio_player.h"

void AudioPlayer_Init(struct audio_player *self_p)
{
    assert(self_p);

    *self_p = (__typeof__(*self_p)){0};
}

bool AudioPlayer_IsPlaying(const struct audio_player *self_p)
{
    assert(self_p);
    return self_p->current_sound_p != NULL;
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

    return self_p->current_position < self_p->current_sound_p->size;
}

uint8_t AudioPlayer_GetNextSample(struct audio_player *self_p)
{
    assert(self_p);

    uint8_t sample = 0;

    if (AudioPlayer_HasMoreSamples(self_p))
    {
        sample = self_p->current_sound_p->samples[self_p->current_position];
        self_p->current_position += 1;
    }

    return sample;
}
