
#ifndef AUDIOLIBRARY_H_
#define AUDIOLIBRARY_H_

#include <stddef.h>
#include <stdint.h>

{id_defines}

struct sound
{{
    size_t size;
    uint8_t samples[0];
}};

const struct sound *AudioLibrary_GetSoundById(size_t sound_id);

#endif
