
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <avr/pgmspace.h>

#include "audio_library.h"
#include "audio_data.inc"

#define CastToSoundPtr(ptr) ((struct sound*)(ptr))


const struct sound *AudioLibrary_GetSoundById(size_t sound_id)
{
    switch(sound_id)
    {
        case AUDIOLIBRARY_HELLO_ID:
            return CastToSoundPtr(&hello);

        case AUDIOLIBRARY_NAME_ID:
            return CastToSoundPtr(&name);

        case AUDIOLIBRARY_KERSTIN_ID:
            return CastToSoundPtr(&kerstin);

        default:
            assert(NULL && "Invalid ID - no matching sound");
    }
}
