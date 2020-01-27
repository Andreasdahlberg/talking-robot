
#ifndef AUDIOPLAYER_H_
#define AUDIOPLAYER_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "audio_library.h"

struct audio_player
{
    const struct sound *current_sound_p;
    size_t current_position;
};

void AudioPlayer_Init(struct audio_player *self_p);
bool AudioPlayer_IsPlaying(const struct audio_player *self_p);
void AudioPlayer_Play(struct audio_player *self_p, const struct sound *sound_p);
void AudioPlayer_Stop(struct audio_player *self_p);
bool AudioPlayer_HasMoreSamples(const struct audio_player *self_p);
uint8_t AudioPlayer_GetNextSample(struct audio_player *self_p);

#endif
