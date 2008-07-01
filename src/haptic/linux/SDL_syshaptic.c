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

#ifdef SDL_JOYSTICK_LINUX

#include "SDL_haptic.h"
#include "../SDL_haptic_c.h"
#include "../SDL_syshaptic.h"

#include <unistd.h> /* close */
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <string.h>
#include <errno.h>


#define MAX_HAPTICS  32


/*
 * List of available haptic devices.
 */
static struct
{
   char *fname;
   SDL_Haptic *haptic;
} SDL_hapticlist[MAX_HAPTICS];


/*
 * Haptic system hardware data.
 */
struct haptic_hwdata
{
   int fd;
};


/*
 * Haptic system effect data.
 */
struct haptic_hweffect
{
   struct ff_effect effect;
};



#define test_bit(nr, addr) \
   (((1UL << ((nr) & 31)) & (((const unsigned int *) addr)[(nr) >> 5])) != 0)
#define EV_TEST(ev,f) \
   if (test_bit((ev), features)) ret |= (f);
/*
 * Test whether a device has haptic properties.
 * Returns available properties or 0 if there are none.
 */
static int
EV_IsHaptic(int fd)
{
   unsigned int ret;
   unsigned long features[1 + FF_MAX/sizeof(unsigned long)];

   ret = 0;

   ioctl(fd, EVIOCGBIT(EV_FF, sizeof(unsigned long) * 4), features);

   EV_TEST(FF_CONSTANT,   SDL_HAPTIC_CONSTANT);
   EV_TEST(FF_PERIODIC,   SDL_HAPTIC_PERIODIC);
   EV_TEST(FF_RAMP,       SDL_HAPTIC_RAMP);
   EV_TEST(FF_SPRING,     SDL_HAPTIC_SPRING);
   EV_TEST(FF_FRICTION,   SDL_HAPTIC_FRICTION);
   EV_TEST(FF_DAMPER,     SDL_HAPTIC_DAMPER);
   EV_TEST(FF_RUMBLE,     SDL_HAPTIC_RUMBLE);
   EV_TEST(FF_INERTIA,    SDL_HAPTIC_INERTIA);
   EV_TEST(FF_GAIN,       SDL_HAPTIC_GAIN);
   EV_TEST(FF_AUTOCENTER, SDL_HAPTIC_AUTOCENTER);

   return ret;
}

int
SDL_SYS_HapticInit(void)
{
   const char joydev_pattern[] = "/dev/input/event%d";
   dev_t dev_nums[MAX_HAPTICS];
   char path[PATH_MAX];
   struct stat sb;
   int fd;
   int i, j, k;
   int duplicate;
   int numhaptics;

   numhaptics = 0;

   i = 0;
   for (j = 0; j < MAX_HAPTICS; ++j) {

      snprintf(path, PATH_MAX, joydev_pattern, i++);

      /* check to see if file exists */
      if (stat(path,&sb) != 0)
         break;

      /* check for duplicates */
      duplicate = 0;
      for (k = 0; (k < numhaptics) && !duplicate; ++k) {
         if (sb.st_rdev == dev_nums[k]) {
            duplicate = 1;
         }                                                     
      }                                                         
      if (duplicate) {
         continue;
      }

      /* try to open */
      fd = open(path, O_RDWR, 0);
      if (fd < 0) continue;

#ifdef DEBUG_INPUT_EVENTS
      printf("Checking %s\n", path);
#endif          

      /* see if it works */
      if (EV_IsHaptic(fd)!=0) {
         SDL_hapticlist[numhaptics].fname = SDL_strdup(path);
         SDL_hapticlist[numhaptics].haptic = NULL;
         dev_nums[numhaptics] = sb.st_rdev;
         ++numhaptics;
      }
      close(fd);
   }

   return numhaptics;
}


/*
 * Return the name of a haptic device, does not need to be opened.
 */
const char *
SDL_SYS_HapticName(int index)
{
   int fd;
   static char namebuf[128];
   char *name;

   name = NULL;
   fd = open(SDL_hapticlist[index].fname, O_RDONLY, 0);
   if (fd >= 0) {
      if (ioctl(fd, EVIOCGNAME(sizeof(namebuf)), namebuf) <= 0) {
         name = SDL_hapticlist[index].fname;
      }
      else {
         name = namebuf;
      }
   }
   close(fd);

   return name;
}


/*
 * Opens a haptic device for usage.
 */
int
SDL_SYS_HapticOpen(SDL_Haptic * haptic)
{
   int i;
   int fd;

   /* Open the character device */
   fd = open(SDL_hapticlist[haptic->index].fname, O_RDWR, 0);
   if (fd < 0) {
      SDL_SetError("Unable to open %s\n", SDL_hapticlist[haptic->index]);
      return -1;
   }

   /* Allocate the hwdata */
   haptic->hwdata = (struct haptic_hwdata *)
         SDL_malloc(sizeof(*haptic->hwdata));
   if (haptic->hwdata == NULL) {
      SDL_OutOfMemory();
      goto open_err;
   }
   SDL_memset(haptic->hwdata, 0, sizeof(*haptic->hwdata));
   /* Set the data */
   haptic->hwdata->fd = fd;
   haptic->supported = EV_IsHaptic(fd);

   /* Set the effects */
   if (ioctl(fd, EVIOCGEFFECTS, &haptic->neffects) < 0) {
      SDL_SetError("Unable to query haptic device memory.");
      goto open_err;
   }
   haptic->effects = (struct haptic_effect *)
         SDL_malloc(sizeof(struct haptic_effect) * haptic->neffects);
   if (haptic->effects == NULL) {
      SDL_OutOfMemory();
      goto open_err;
   }

   return 0;

   /* Error handling */
open_err:
   close(fd);
   if (haptic->hwdata != NULL) {
      free(haptic->hwdata);
      haptic->hwdata = NULL;
   }
   return -1;
}


/*
 * Closes the haptic device.
 */
void
SDL_SYS_HapticClose(SDL_Haptic * haptic)
{
   if (haptic->hwdata) {

      /* Clean up */
      close(haptic->hwdata->fd);

      /* Free */
      SDL_free(haptic->hwdata);
      haptic->hwdata = NULL;
      SDL_free(haptic->effects);
      haptic->neffects = 0;
   }
}


/* 
 * Clean up after system specific haptic stuff
 */
void
SDL_SYS_HapticQuit(void)
{
   int i;

   for (i=0; SDL_hapticlist[i].fname != NULL; i++) {
      SDL_free(SDL_hapticlist[i].fname);
   }
   SDL_hapticlist[0].fname = NULL;
}

/*
 * Initializes the linux effect struct from a haptic_effect.
 */
static int
SDL_SYS_ToFFEffect( struct ff_effect * dest, SDL_HapticEffect * src )
{
   SDL_HapticConstant *constant;
   SDL_HapticPeriodic *periodic;

   /* Clear up */
   SDL_memset(dest, 0, sizeof(struct ff_effect));

   switch (src->type) {
      case SDL_HAPTIC_CONSTANT:
         constant = &src->constant;

         /* Header */
         dest->type = FF_CONSTANT;
         dest->direction = constant->direction;

         /* Replay */
         dest->replay.length = constant->length;
         dest->replay.delay = constant->delay;

         /* Trigger */
         dest->trigger.button = constant->button;
         dest->trigger.interval = constant->interval;

         /* Constant */
         dest->u.constant.level = constant->level;

         /* Envelope */
         dest->u.constant.envelope.attack_length = constant->attack_length;
         dest->u.constant.envelope.attack_level = constant->attack_level;
         dest->u.constant.envelope.fade_length = constant->fade_length;
         dest->u.constant.envelope.fade_level = constant->fade_level;

         break;

      case SDL_HAPTIC_PERIODIC:
         periodic = &src->periodic;

         /* Header */
         dest->type = FF_PERIODIC;
         dest->direction = periodic->direction;
         
         /* Replay */
         dest->replay.length = periodic->length;
         dest->replay.delay = periodic->delay;
         
         /* Trigger */
         dest->trigger.button = periodic->button;
         dest->trigger.interval = periodic->interval;
         
         /* Constant */
         dest->u.periodic.waveform = periodic->waveform;
         dest->u.periodic.period = periodic->period;
         dest->u.periodic.magnitude = periodic->magnitude;
         dest->u.periodic.offset = periodic->offset;
         dest->u.periodic.phase = periodic->phase;
         
         /* Envelope */
         dest->u.periodic.envelope.attack_length = periodic->attack_length;
         dest->u.periodic.envelope.attack_level = periodic->attack_level;
         dest->u.periodic.envelope.fade_length = periodic->fade_length;
         dest->u.periodic.envelope.fade_level = periodic->fade_level;

         break;

      default:
         SDL_SetError("Unknown haptic effect type.");
         return -1;
   }

   return 0;
}

/*
 * Creates a new haptic effect.
 */
int
SDL_SYS_HapticNewEffect(SDL_Haptic * haptic, struct haptic_effect * effect,
      SDL_HapticEffect * base)
{
   struct ff_effect * linux_effect;

   /* Allocate the hardware effect */
   effect->hweffect = (struct haptic_hweffect *) 
         SDL_malloc(sizeof(struct haptic_hweffect));
   if (effect->hweffect == NULL) {
      SDL_OutOfMemory();
      return -1;
   }

   /* Prepare the ff_effect */
   linux_effect = &effect->hweffect->effect;
   if (SDL_SYS_ToFFEffect( linux_effect, base ) != 0) {
      return -1;
   }
   linux_effect->id = -1; /* Have the kernel give it an id */

   /* Upload the effect */
   if (ioctl(haptic->hwdata->fd, EVIOCSFF, linux_effect) < 0) {
      SDL_SetError("Error uploading effect to the haptic device: %s",
            strerror(errno));
      return -1;
   }

   return 0;
}


/*
 * Runs an effect.
 */
int
SDL_SYS_HapticRunEffect(SDL_Haptic * haptic, struct haptic_effect * effect)
{
   struct input_event run;

   /* Prepare to run the effect */
   run.type = EV_FF;
   run.code = effect->hweffect->effect.id;
   run.value = 1;

   if (write(haptic->hwdata->fd, (const void*) &run, sizeof(run)) < 0) {
      SDL_SetError("Unable to run the haptic effect.");
      return -1;
   }

   return 0;
}


/*
 * Frees the effect
 */
void
SDL_SYS_HapticDestroyEffect(SDL_Haptic * haptic, struct haptic_effect * effect)
{
   if (ioctl(haptic->hwdata->fd, EVIOCRMFF, effect->hweffect->effect.id) < 0) {
      SDL_SetError("Error removing the effect from the haptic device.");
   }
   SDL_free(effect->hweffect);
   effect->hweffect = NULL;
}


#endif /* SDL_HAPTIC_LINUX */
