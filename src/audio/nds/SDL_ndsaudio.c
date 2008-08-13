/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org

    This file written by Ryan C. Gordon (icculus@icculus.org)
*/
#include "SDL_config.h"

/* Output audio to NDS */

#include <nds.h>

#include "SDL_audio.h"
#include "../SDL_audio_c.h"
#include "SDL_ndsaudio.h"

static int
NDSAUD_OpenDevice(_THIS, const char *devname, int iscapture)
{
    return 1;                   /* always succeeds. */
}

static void
NDSAUD_PlayDevice(_THIS)
{
    TransferSoundData* sound = SDL_malloc(sizeof(TransferSoundData));
    if(!sound) {
        SDL_OutOfMemory();
    }
    sound->data = NULL;  /* pointer to raw audio data */
    sound->len = 0;      /* size of raw data pointed to above */
    sound->rate = 22050; /* sample rate = 22050Hz */
    sound->vol = 127;    /* volume [0..127] for [min..max] */
    sound->pan = 64;     /* balance [0..127] for [left..right] */
    sound->format = 0;   /* 0 for 16-bit, 1 for 8-bit */
    /*playSound(sound);*/
    /* stub */
}


static Uint8 *
NDSAUD_GetDeviceBuf(_THIS)
{
    /* stub */
}

static void
NDSAUD_WaitDevice(_THIS)
{
    /* stub */
}

static void
NDSAUD_CloseDevice(_THIS)
{
    /* stub */
}

static int
NDSAUD_Init(SDL_AudioDriverImpl * impl)
{
    /* Set the function pointers */
    impl->OpenDevice = NDSAUD_OpenDevice;
    impl->PlayDevice = NDSAUD_PlayDevice;
    impl->WaitDevice = NDSAUD_WaitDevice;
    impl->GetDeviceBuf = NDSAUD_GetDeviceBuf;
    impl->CloseDevice = NDSAUD_CloseDevice;
    impl->OnlyHasDefaultOutputDevice = 1;
    return 1;
}

AudioBootStrap NDSAUD_bootstrap = {
    "nds", "SDL NDS audio driver", NDSAUD_Init, 1
};

/* vi: set ts=4 sw=4 expandtab: */
