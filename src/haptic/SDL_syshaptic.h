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


struct haptic_effect
{
   SDL_HapticEffect effect; /* The current event */
   struct haptic_hweffect *hweffect; /* The hardware behind the event */
};

/*
 * The real SDL_Haptic event.
 */
struct _SDL_Haptic
{  
   Uint8 index; /* Stores index it is attached to */
   const char* name; /* Stores the name of the device */

   struct haptic_effect *effects; /* Allocated effects */
   int neffects; /* Maximum amount of effects */
   unsigned int supported; /* Supported effects */

   struct haptic_hwdata *hwdata; /* Driver dependent */
   int ref_count; /* Count for multiple opens */
};

extern int SDL_SYS_HapticInit(void);
extern const char * SDL_SYS_HapticName(int index);
extern int SDL_SYS_HapticOpen(SDL_Haptic * haptic);
extern void SDL_SYS_HapticClose(SDL_Haptic * haptic);
extern void SDL_SYS_HapticQuit(void);
extern int SDL_SYS_HapticNewEffect(SDL_Haptic * haptic,
      struct haptic_effect * effect, SDL_HapticEffect * base);
extern int SDL_SYS_HapticRunEffect(SDL_Haptic * haptic,
      struct haptic_effect * effect);
extern void SDL_SYS_HapticDestroyEffect(SDL_Haptic * haptic,
      struct haptic_effect * effect);


