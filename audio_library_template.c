
#include <assert.h>
#include <avr/pgmspace.h>

#include "audio_library.h"
{includes}

#define CastToSoundPtr(ptr) ((struct sound*)(ptr))

const struct sound *AudioLibrary_GetSoundById(size_t sound_id)
{{
    switch(sound_id)
    {{
{cases}
        default:
            assert(NULL && "Invalid ID - no matching sound");
    }}
}}
