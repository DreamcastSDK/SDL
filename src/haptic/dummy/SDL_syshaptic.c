/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 2008 Edgar Simo

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
*/
#include "SDL_config.h"

#ifdef defined(SDL_HAPTIC_DUMMY) || defined(SDL_HAPTIC_DISABLED)

#include "SDL_haptic.h"
#include "../SDL_haptic_c.h"
#include "../SDL_syshaptic.h"


int
SDL_SYS_HapticInit(void)
{
   SDL_numhaptics = 0;
   return (0);
}


const char *
SDL_SYS_HapticName(int index)
{
   SDL_SetError("Logic error: No haptic (force feedback) devices available");
   return (NULL);
}


int
SDL_SYS_HapticOpen(SDL_Haptic * haptic)
{
   SDL_SetError("Logic error: No joysticks available");
   return (-1);
}


void
SDL_SYS_HapticClose(SDL_Haptic * haptic)
{
   return;
}


void
SDL_SYS_HapticQuit(void)
{
   return;
}


#endif /* SDL_HAPTIC_DUMMY || SDL_HAPTIC_DISABLED */
