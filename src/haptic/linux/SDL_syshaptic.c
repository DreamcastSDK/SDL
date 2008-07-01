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
   EV_TEST(FF_PERIODIC,   SDL_HAPTIC_SINE |
                          SDL_HAPTIC_SQUARE |
                          SDL_HAPTIC_TRIANGLE |
                          SDL_HAPTIC_SAWTOOTHUP |
                          SDL_HAPTIC_SAWTOOTHDOWN);
   EV_TEST(FF_RAMP,       SDL_HAPTIC_RAMP);
   EV_TEST(FF_SPRING,     SDL_HAPTIC_SPRING);
   EV_TEST(FF_FRICTION,   SDL_HAPTIC_FRICTION);
   EV_TEST(FF_DAMPER,     SDL_HAPTIC_DAMPER);
   EV_TEST(FF_INERTIA,    SDL_HAPTIC_INERTIA);
   EV_TEST(FF_CUSTOM,     SDL_HAPTIC_CUSTOM);
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
   /* Clear the memory */
   SDL_memset(haptic->effects, 0,
         sizeof(struct haptic_effect) * haptic->neffects);

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

#define  CLAMP(x)    (((x) > 32767) ? 32767 : x)
/*
 * Initializes the linux effect struct from a haptic_effect.
 * Values above 32767 (for unsigned) are unspecified so we must clamp.
 */
static int
SDL_SYS_ToFFEffect( struct ff_effect * dest, SDL_HapticEffect * src )
{
   int i;
   SDL_HapticConstant *constant;
   SDL_HapticPeriodic *periodic;
   SDL_HapticCondition *condition;
   SDL_HapticRamp *ramp;

   /* Clear up */
   SDL_memset(dest, 0, sizeof(struct ff_effect));

   switch (src->type) {
      case SDL_HAPTIC_CONSTANT:
         constant = &src->constant;

         /* Header */
         dest->type = FF_CONSTANT;
         dest->direction = CLAMP(constant->direction);

         /* Replay */
         dest->replay.length = CLAMP(constant->length);
         dest->replay.delay = CLAMP(constant->delay);

         /* Trigger */
         dest->trigger.button = CLAMP(constant->button);
         dest->trigger.interval = CLAMP(constant->interval);

         /* Constant */
         dest->u.constant.level = constant->level;

         /* Envelope */
         dest->u.constant.envelope.attack_length = CLAMP(constant->attack_length);
         dest->u.constant.envelope.attack_level = CLAMP(constant->attack_level);
         dest->u.constant.envelope.fade_length = CLAMP(constant->fade_length);
         dest->u.constant.envelope.fade_level = CLAMP(constant->fade_level);

         break;

      case SDL_HAPTIC_SINE:
      case SDL_HAPTIC_SQUARE:
      case SDL_HAPTIC_TRIANGLE:
      case SDL_HAPTIC_SAWTOOTHUP:
      case SDL_HAPTIC_SAWTOOTHDOWN:
         periodic = &src->periodic;

         /* Header */
         dest->type = FF_PERIODIC;
         dest->direction = CLAMP(periodic->direction);
         
         /* Replay */
         dest->replay.length = CLAMP(periodic->length);
         dest->replay.delay = CLAMP(periodic->delay);
         
         /* Trigger */
         dest->trigger.button = CLAMP(periodic->button);
         dest->trigger.interval = CLAMP(periodic->interval);
         
         /* Periodic */
         if (periodic->type == SDL_HAPTIC_SINE)
            dest->u.periodic.waveform = FF_SINE;
         else if (periodic->type == SDL_HAPTIC_SQUARE)
            dest->u.periodic.waveform = FF_SQUARE;
         else if (periodic->type == SDL_HAPTIC_TRIANGLE)       
            dest->u.periodic.waveform = FF_TRIANGLE;
         else if (periodic->type == SDL_HAPTIC_SAWTOOTHUP)       
            dest->u.periodic.waveform = FF_SAW_UP;
         else if (periodic->type == SDL_HAPTIC_SAWTOOTHDOWN)       
            dest->u.periodic.waveform = FF_SAW_DOWN;
         dest->u.periodic.period = CLAMP(periodic->period);
         dest->u.periodic.magnitude = periodic->magnitude;
         dest->u.periodic.offset = periodic->offset;
         dest->u.periodic.phase = CLAMP(periodic->phase);
         
         /* Envelope */
         dest->u.periodic.envelope.attack_length = CLAMP(periodic->attack_length);
         dest->u.periodic.envelope.attack_level = CLAMP(periodic->attack_level);
         dest->u.periodic.envelope.fade_length = CLAMP(periodic->fade_length);
         dest->u.periodic.envelope.fade_level = CLAMP(periodic->fade_level);

         break;

      case SDL_HAPTIC_SPRING:
      case SDL_HAPTIC_DAMPER:
      case SDL_HAPTIC_INERTIA:
      case SDL_HAPTIC_FRICTION:
         condition = &src->condition;

         /* Header */
         if (dest->type == SDL_HAPTIC_SPRING)
            dest->type = FF_SPRING;
         else if (dest->type == SDL_HAPTIC_DAMPER)
            dest->type = FF_DAMPER;
         else if (dest->type == SDL_HAPTIC_INERTIA)
            dest->type = FF_INERTIA;
         else if (dest->type == SDL_HAPTIC_FRICTION)
            dest->type = FF_FRICTION;
         dest->direction = CLAMP(condition->direction);

         /* Replay */
         dest->replay.length = CLAMP(condition->length);
         dest->replay.delay = CLAMP(condition->delay);

         /* Trigger */
         dest->trigger.button = CLAMP(condition->button);
         dest->trigger.interval = CLAMP(condition->interval);

         /* Condition - TODO handle axes */
         dest->u.condition[0].right_saturation = CLAMP(condition->right_sat);
         dest->u.condition[0].left_saturation = CLAMP(condition->left_sat);
         dest->u.condition[0].right_coeff = condition->right_coeff;
         dest->u.condition[0].left_coeff = condition->left_coeff;
         dest->u.condition[0].deadband = CLAMP(condition->deadband);
         dest->u.condition[0].center = condition->center;

         break;

      case SDL_HAPTIC_RAMP:
         ramp = &src->ramp;

         /* Header */
         dest->type = FF_RAMP;
         dest->direction = CLAMP(ramp->direction);

         /* Replay */
         dest->replay.length = CLAMP(ramp->length);
         dest->replay.delay = CLAMP(ramp->delay);

         /* Trigger */
         dest->trigger.button = CLAMP(ramp->button);
         dest->trigger.interval = CLAMP(ramp->interval);

         /* Ramp */
         dest->u.ramp.start_level = ramp->start;
         dest->u.ramp.end_level = ramp->end;

         /* Envelope */
         dest->u.ramp.envelope.attack_length = CLAMP(ramp->attack_length);
         dest->u.ramp.envelope.attack_level = CLAMP(ramp->attack_level);
         dest->u.ramp.envelope.fade_length = CLAMP(ramp->fade_length);
         dest->u.ramp.envelope.fade_level = CLAMP(ramp->fade_level);

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
      SDL_SetError("Error uploading effect to the haptic device.");
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
 * Stops an effect.
 */
int
SDL_SYS_HapticStopEffect(SDL_Haptic * haptic, struct haptic_effect * effect)
{
   struct input_event stop;

   stop.type = EV_FF;
   stop.code = effect->hweffect->effect.id;
   stop.value = 0;

   if (write(haptic->hwdata->fd, (const void*) &stop, sizeof(stop)) < 0) {
      SDL_SetError("Unable to stop the haptic effect.");
      return -1;
   }

   return 0;
}


/*
 * Frees the effect.
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


/*
 * Sets the gain.
 */
int
SDL_SYS_HapticSetGain(SDL_Haptic * haptic, int gain)
{
   struct input_event ie;

   ie.type = EV_FF;
   ie.code = FF_GAIN;
   ie.value = (0xFFFFUL * gain) / 100;

   if (write(haptic->hwdata->fd, &ie, sizeof(ie)) < 0) {
      SDL_SetError("Error setting gain.");
      return -1;
   }

   return 0;
}


/*
 * Sets the autocentering.
 */
int
SDL_SYS_HapticSetAutocenter(SDL_Haptic * haptic, int autocenter)
{
   struct input_event ie;

   ie.type = EV_FF;
   ie.code = FF_AUTOCENTER;
   ie.value = (0xFFFFUL * autocenter) / 100;

   if (write(haptic->hwdata->fd, &ie, sizeof(ie)) < 0) {
      SDL_SetError("Error setting autocenter.");
      return -1;
   }

   return 0;
}



#endif /* SDL_HAPTIC_LINUX */
