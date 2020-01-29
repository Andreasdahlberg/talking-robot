
#ifndef AUDIOLIBRARY_H_
#define AUDIOLIBRARY_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#define AUDIOLIBRARY_HELLO_ID    0
#define AUDIOLIBRARY_BYE_ID      1
#define AUDIOLIBRARY_ESPEAK_ID   2

struct sound
{
    size_t size;
    uint8_t samples[0];
};

const struct sound *AudioLibrary_GetSoundById(size_t sound_id);

#endif
