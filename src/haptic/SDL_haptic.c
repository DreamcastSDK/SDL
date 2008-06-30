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

#include "SDL_haptic_c.h"
#include "SDL_syshaptic.h"


static Uint8 SDL_numhaptics = 0;
SDL_Haptic **SDL_haptics = NULL;
static SDL_Haptic *default_haptic = NULL;


/*
 * Initializes the Haptic devices.
 */
int
SDL_HapticInit(void)
{  
   int arraylen;
   int status;

   SDL_numhaptics = 0;
   status = SDL_SYS_HapticInit();
   if (status >= 0) {
      arraylen = (status + 1) * sizeof(*SDL_haptics);
      SDL_haptics = (SDL_Haptic **) SDL_malloc(arraylen);
      if (SDL_haptics == NULL) {
         SDL_numhaptics = 0;
      } else {
         SDL_memset(SDL_haptics, 0, arraylen);
         SDL_numhaptics = status;
      }
      status = 0;
   }
   default_haptic = NULL;

   return status;
}


/*
 * Returns the number of available devices.
 */
int
SDL_NumHaptics(void)
{
   return SDL_numhaptics;
}


/*
 * Gets the name of a Haptic device by index.
 */
const char *
SDL_HapticName(int device_index)
{
   if ((device_index < 0) || (device_index >= SDL_numhaptics)) {
      SDL_SetError("There are %d haptic devices available", SDL_numhaptics);
      return NULL;
   }
   return SDL_SYS_HapticName(device_index);
}


/*
 * Opens a Haptic device.
 */
SDL_Haptic *
SDL_HapticOpen(int device_index)
{
   int i;
   SDL_Haptic *haptic;

   if ((device_index < 0) || (device_index >= SDL_numhaptics)) {
      SDL_SetError("There are %d haptic devices available", SDL_numhaptics);
      return NULL;
   }

   /* If the haptic is already open, return it */
   for (i=0; SDL_haptics[i]; i++) {             
      if (device_index == SDL_haptics[i]->index) {
         haptic = SDL_haptics[i];
         ++haptic->ref_count;
         return haptic;
      }
   }

   /* Create the haptic device */
   haptic = (SDL_Haptic *) SDL_malloc((sizeof *haptic));
   if (haptic == NULL) {
      SDL_OutOfMemory();
      return NULL;
   }

   /* Initialize the haptic device */
   SDL_memset(haptic, 0, (sizeof *haptic));
   haptic->index = device_index;
   if (SDL_SYS_HapticOpen(haptic) < 0) {
      SDL_free(haptic);
      return NULL;
   }

   /* Add haptic to list */
   ++haptic->ref_count;
   for (i = 0; SDL_haptics[i]; ++i)
      /* Skip to next haptic */ ;
   SDL_haptics[i] = haptic;

   return haptic;
}


/*
 * Checks to see if the haptic device is valid
 */
static int
ValidHaptic(SDL_Haptic ** haptic)
{
   int valid;

   if (*haptic == NULL) {
      SDL_SetError("Haptic device hasn't been opened yet");
      valid = 0;
   } else {
      valid = 1;
   }
   return valid;
}


/*
 * Closes a SDL_Haptic device.
 */
void
SDL_HapticClose(SDL_Haptic * haptic)
{
   int i;

   /* Must be valid */
   if (!ValidHaptic(&haptic)) {
      return;
   }

   /* Check if it's still in use */
   if (--haptic->ref_count < 0) {
      return;
   }

   /* Close it */
   SDL_SYS_HapticClose(haptic);

   /* Remove from the list */
   for (i = 0; SDL_haptics[i]; ++i) {
      if (haptic == SDL_haptics[i]) {
         SDL_memcpy(&SDL_haptics[i], &SDL_haptics[i + 1],
               (SDL_numhaptics - i) * sizeof(haptic));
         break;
      }
   }

   /* Free */
   SDL_free(haptic);
}

/*
 * Cleans up after the subsystem.
 */
void
SDL_HapticQuit(void)
{
   SDL_numhaptics = 0;

   SDL_SYS_HapticQuit();
   if (SDL_haptics != NULL) {
      SDL_free(SDL_haptics);
      SDL_haptics = NULL;
   }
}

/*
 * Returns the number of effects a haptic device has.
 */
int
SDL_HapticNumEffects(SDL_Haptic * haptic)
{
   if (!ValidHaptic(&haptic)) {
      return -1;
   }

   return haptic->neffects;
}

/*
 * Returns supported effects by the device.
 */
unsigned int
SDL_HapticQueryEffects(SDL_Haptic * haptic)
{
   if (!ValidHaptic(&haptic)) {
      return -1;
   }

   return haptic->supported;
}

int
SDL_HapticEffectSupported(SDL_Haptic * haptic, SDL_HapticEffect * effect)
{
   if (!ValidHaptic(&haptic)) {
      return -1;
   }

   if ((haptic->supported & effect->type) != 0)
      return SDL_TRUE;
   return SDL_FALSE;
}

/*
 * Creates a new haptic effect.
 */
int
SDL_HapticNewEffect(SDL_Haptic * haptic, SDL_HapticEffect * effect)
{
   int i;

   /* Check for device validity. */
   if (!ValidHaptic(&haptic)) {
      return -1;
   }

   /* See if there's a free slot */
   for (i=0; i<haptic->neffects; i++) {
      if (haptic->effects[i].hweffect == NULL) {

         /* Now let the backend create the real effect */
         if (SDL_SYS_HapticNewEffect(haptic,&haptic->effects[i]) != 0) {
            return -1; /* Backend failed to create effect */
         }
         return i;
      }
   }

   SDL_SetError("Haptic device has no free space left.");
   return -1;
}

/*
 * Runs the haptic effect on the device.
 */
int
SDL_HapticRunEffect(SDL_Haptic * haptic, int effect)
{
   if (!ValidHaptic(&haptic)) {
      return -1;
   }

   /* Run the effect */
   if (SDL_SYS_HapticRunEffect(haptic,&haptic->effects[effect]) < 0) {
      return -1;
   }

   return 0;
}

/*
 * Gets rid of a haptic effect.
 */
void
SDL_HapticDestroyEffect(SDL_Haptic * haptic, int effect)
{
   if (!ValidHaptic(&haptic)) {
      return;
   }

   /* Not allocated */
   if (haptic->effects[effect].hweffect == NULL) {
      return;
   }

   SDL_SYS_HapticDestroyEffect(haptic, &haptic->effects[effect]);
}


