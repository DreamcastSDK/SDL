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

/* Output audio to nowhere... */

#include "SDL_rwops.h"
#include "SDL_timer.h"
#include "SDL_audio.h"
#include "../SDL_audiomem.h"
#include "../SDL_audio_c.h"
#include "SDL_dummyaudio.h"

/* The tag name used by DUMMY audio */
#define DUMMYAUD_DRIVER_NAME         "dummy"

/* Audio driver functions */
static int DUMMYAUD_OpenDevice(_THIS, const char *devname, int iscapture);
static void DUMMYAUD_WaitDevice(_THIS);
static void DUMMYAUD_PlayDevice(_THIS);
static Uint8 *DUMMYAUD_GetDeviceBuf(_THIS);
static void DUMMYAUD_CloseDevice(_THIS);

/* Audio driver bootstrap functions */
static int
DUMMYAUD_Available(void)
{
    return 1;  /* always available. */
}

static int
DUMMYAUD_Init(SDL_AudioDriverImpl *impl)
{
    /* Set the function pointers */
    impl->OpenDevice = DUMMYAUD_OpenDevice;
    impl->OnlyHasDefaultOutputDevice = 1;

    return 1;
}

AudioBootStrap DUMMYAUD_bootstrap = {
    DUMMYAUD_DRIVER_NAME, "SDL dummy audio driver",
    DUMMYAUD_Available, DUMMYAUD_Init, 1
};

static int
DUMMYAUD_OpenDevice(_THIS, const char *devname, int iscapture)
{
    return 1;   /* always succeeds. */
}

/* vi: set ts=4 sw=4 expandtab: */
