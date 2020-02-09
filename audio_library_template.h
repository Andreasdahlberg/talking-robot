
#ifndef AUDIOLIBRARY_H_
#define AUDIOLIBRARY_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

{id_defines}

#define AUDIOLIBRARY_DELAY_ID(delay_ms) (AUDIOLIBRARY_MAX_ID + (delay_ms))

struct sound
{{
    size_t size;
    uint8_t samples[0];
}};

const struct sound *AudioLibrary_GetSoundById(size_t sound_id);
bool AudioLibrary_IsValidId(size_t sound_id);

#endif
