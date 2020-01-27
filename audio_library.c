
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "audio_library.h"

#define CastToSoundPtr(ptr) ((struct sound*)(ptr))

struct sound_hello
{
    struct sound base;
    uint8_t data[4];
};

struct sound_bye
{
    struct sound base;
    uint8_t data[2];
};

static struct sound_hello hello = {{4}, {1, 2, 3, 4}};
static struct sound_bye bye = {{2}, {6, 7}};

const struct sound *AudioLibrary_GetSoundById(size_t sound_id)
{
    switch(sound_id)
    {
        case AUDIOLIBRARY_HELLO_ID:
            return CastToSoundPtr(&hello);

        case AUDIOLIBRARY_BYE_ID:
            return CastToSoundPtr(&bye);

        default:
            assert(NULL && "Invalid ID - no matching sound");
    }
}
