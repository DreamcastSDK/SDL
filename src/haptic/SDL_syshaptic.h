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

#include "SDL_haptic.h"


struct _SDL_Haptic
{  
   Uint8 index; /* stores index it is attached to */
   const char* name; /* stores the name of the device */

   int neffects; /* maximum amount of effects */
   unsigned int supported; /* supported effects */

   struct haptic_hwdata *hwdata; /* driver dependent */
   int ref_count; /* count for multiple opens */
};


extern int SDL_SYS_HapticInit(void);
extern const char * SDL_SYS_HapticName(int index);
extern int SDL_SYS_HapticOpen(SDL_Haptic * haptic);
extern void SDL_SYS_HapticClose(SDL_Haptic * haptic);
extern void SDL_SYS_HapticQuit(void);

