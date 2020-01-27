
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "audio_player.h"

static struct audio_player player;

void PWM_SetDutyCycle(uint8_t duty_cycle)
{
    printf("%s: %u%%\n", __func__, duty_cycle);
}

void InterruptHandler(void)
{
    if (AudioPlayer_IsPlaying(&player))
    {
        if (AudioPlayer_HasMoreSamples(&player))
        {
            uint8_t sample = AudioPlayer_GetNextSample(&player);
            PWM_SetDutyCycle(sample);
        }
        else
        {
            AudioPlayer_Stop(&player);
        }
    }
}

int main()
{
    AudioPlayer_Init(&player);
    AudioPlayer_Play(&player, AudioLibrary_GetSoundById(AUDIOLIBRARY_HELLO_ID));

    while(AudioPlayer_IsPlaying(&player))
    {
        InterruptHandler();
    }

    return 0;
}
