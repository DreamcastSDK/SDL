/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2009 Sam Lantinga

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

/* SDL PS3 video driver implementation based on dummy video driver
 *
 * Initial work by Ryan C. Gordon (icculus@icculus.org). A good portion
 *  of this was cut-and-pasted from Stephane Peter's work in the AAlib
 *  SDL video driver.  Renamed to "DUMMY" by Sam Lantinga.
 */

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"
#include "spulibs/spu_common.h"

#include "SDL_ps3video.h"
#include "SDL_ps3events_c.h"
#include "SDL_ps3render_c.h"

#include <fcntl.h>
#include <linux/fb.h>
#include <asm/ps3fb.h>
#include <libspe2.h>
#include <sys/mman.h>

#define PS3VID_DRIVER_NAME "ps3"

/* Initialization/Query functions */
static int PS3_VideoInit(_THIS);
static int PS3_SetDisplayMode(_THIS, SDL_DisplayMode * mode);
static void PS3_VideoQuit(_THIS);

/* SPU specific functions */
int SPE_Start(_THIS, spu_data_t * spe_data);
int SPE_Stop(_THIS, spu_data_t * spe_data);
int SPE_Boot(_THIS, spu_data_t * spe_data);
int SPE_Shutdown(_THIS, spu_data_t * spe_data);
int SPE_SendMsg(_THIS, spu_data_t * spe_data, unsigned int msg);
int SPE_WaitForMsg(_THIS, spu_data_t * spe_data, unsigned int msg);
void SPE_RunContext(void *thread_argp);

/* Stores the SPE executable name of fb_writer_spu */
extern spe_program_handle_t fb_writer_spu;

/* PS3 driver bootstrap functions */

static int
PS3_Available(void)
{
    deprintf(1, "PS3_Available()\n");
    const char *envr = SDL_getenv("SDL_VIDEODRIVER");
    if ((envr) && (SDL_strcmp(envr, PS3VID_DRIVER_NAME) == 0)) {
        return (1);
    }

    return (0);
}

static void
PS3_DeleteDevice(SDL_VideoDevice * device)
{
    deprintf(1, "PS3_DeleteDevice()\n");
    SDL_free(device->driverdata);
    SDL_free(device);
}

static SDL_VideoDevice *
PS3_CreateDevice(int devindex)
{
    deprintf(1, "PS3_CreateDevice()\n");
    SDL_VideoDevice *device;
    SDL_VideoData *data;

    /* Initialize all variables that we clean on shutdown */
    device = (SDL_VideoDevice *) SDL_calloc(1, sizeof(SDL_VideoDevice));
    if (!device) {
        SDL_OutOfMemory();
        if (device) {
            SDL_free(device);
        }
        return (0);
    }
    data = (struct SDL_VideoData *) SDL_calloc(1, sizeof(SDL_VideoData));
    if (!data) {
        SDL_OutOfMemory();
        SDL_free(device);
        return (0);
    }
    device->driverdata = data;

    /* Set the function pointers */
    device->VideoInit = PS3_VideoInit;
    device->VideoQuit = PS3_VideoQuit;
    device->SetDisplayMode = PS3_SetDisplayMode;
    device->PumpEvents = PS3_PumpEvents;

    device->free = PS3_DeleteDevice;

    return device;
}

VideoBootStrap PS3_bootstrap = {
    PS3VID_DRIVER_NAME, "SDL PS3 Cell video driver",
    PS3_Available, PS3_CreateDevice
};


int
PS3_VideoInit(_THIS)
{
    deprintf(1, "PS3_VideoInit()\n");

    SDL_VideoData *data = (SDL_VideoData *) _this->driverdata;
    SDL_DisplayMode mode;

    /* Use a fake 32-bpp desktop mode */
    mode.format = SDL_PIXELFORMAT_RGB888;
    //mode.w = 1024;
    //mode.h = 768;
    mode.w = 1920;
    mode.h = 1080;
    mode.refresh_rate = 0;
    mode.driverdata = NULL;
    SDL_AddBasicVideoDisplay(&mode);
    SDL_AddRenderDriver(0, &SDL_PS3_RenderDriver);

    SDL_zero(mode);
    SDL_AddDisplayMode(0, &mode);

    /* 
     *PS3 stuff 
     */

    /* Create SPU fb_parms and thread structure */
    data->fb_parms = (struct fb_writer_parms_t *)
        memalign(16, sizeof(struct fb_writer_parms_t));
    data->fb_thread_data = (spu_data_t *) malloc(sizeof(spu_data_t));
    if (data->fb_parms == NULL || data->fb_thread_data == NULL) {
        SDL_OutOfMemory();
        return -1;
    }
    data->fb_thread_data->program = fb_writer_spu;
    data->fb_thread_data->program_name = "fb_writer_spu";
    data->fb_thread_data->argp = (void *)data->fb_parms;
    data->fb_thread_data->keepalive = 1;
    data->fb_thread_data->booted = 0;

    SPE_Start(_this, data->fb_thread_data);

    /* Open the device */
    data->fbdev = open(PS3DEV, O_RDWR);
    if (data->fbdev < 0) {
        SDL_SetError("[PS3] Unable to open device %s", PS3DEV);
        return -1;
    }

    /* Take control of frame buffer from kernel, for details see
     * http://felter.org/wesley/files/ps3/linux-20061110-docs/ApplicationProgrammingEnvironment.html
     * kernel will no longer flip the screen itself
     */
    ioctl(data->fbdev, PS3FB_IOCTL_ON, 0);

    /* Unblank screen */
    ioctl(data->fbdev, FBIOBLANK, 0);

    struct fb_fix_screeninfo fb_finfo;
    if (ioctl(data->fbdev, FBIOGET_FSCREENINFO, &fb_finfo)) {
        SDL_SetError("[PS3] Can't get fixed screeninfo");
        return (0);
    }

    /* Note: on PS3, fb_finfo.smem_len is enough for double buffering */
    if ((data->frame_buffer = (uint8_t *)mmap(0, fb_finfo.smem_len,
        PROT_READ | PROT_WRITE, MAP_SHARED,
        data->fbdev, 0)) == (uint8_t *) - 1) {
        SDL_SetError("[PS3] Can't mmap for %s", PS3DEV);
        return (0);
    } else {
        //current->flags |= SDL_DOUBLEBUF;
    }

    /* Blank screen */
    memset(data->frame_buffer, 0x00, fb_finfo.smem_len);

    /* We're done! */
    return 0;
}

static int
PS3_SetDisplayMode(_THIS, SDL_DisplayMode * mode)
{
    deprintf(1, "PS3_SetDisplayMode()\n");
    return 0;
}

void
PS3_VideoQuit(_THIS)
{
    deprintf(1, "PS3_VideoQuit()\n");
    SDL_VideoData *data = (SDL_VideoData *) _this->driverdata;
    if (data->frame_buffer) {
        struct fb_fix_screeninfo fb_finfo;
        if (ioctl(data->fbdev, FBIOGET_FSCREENINFO, &fb_finfo) != -1) {
            munmap(data->frame_buffer, fb_finfo.smem_len);
            data->frame_buffer = 0;
        }
    }

    if (data->fb_parms)
        free((void *)data->fb_parms);
    if (data->fb_thread_data) {
        SPE_Shutdown(_this, data->fb_thread_data);
        free((void *)data->fb_thread_data);
    }
}


/*
 * SPE handling
 */

/* Start the SPE thread */
int SPE_Start(_THIS, spu_data_t * spe_data)
{
  deprintf(2, "[PS3->SPU] Start SPE: %s\n", spe_data->program_name);
  if (!(spe_data->booted))
    SPE_Boot(_this, spe_data);

  /* To allow re-running of context, spe_ctx_entry has to be set before each call */
  spe_data->entry = SPE_DEFAULT_ENTRY;
  spe_data->error_code = 0;

  /* Create SPE thread and run */
  deprintf(2, "[PS3->SPU] Create Thread: %s\n", spe_data->program_name);
  if (pthread_create
      (&spe_data->thread, NULL, (void *)&SPE_RunContext, (void *)spe_data)) {
    deprintf(2, "[PS3->SPU] Could not create pthread for spe: %s\n", spe_data->program_name);
    SDL_SetError("[PS3->SPU] Could not create pthread for spe");
    return -1;
  }

  if (spe_data->keepalive)
    SPE_WaitForMsg(_this, spe_data, SPU_READY);
}


/* Stop the SPE thread */
int SPE_Stop(_THIS, spu_data_t * spe_data)
{
  deprintf(2, "[PS3->SPU] Stop SPE: %s\n", spe_data->program_name);
  /* Wait for SPE thread to complete */
  deprintf(2, "[PS3->SPU] Wait for SPE thread to complete: %s\n", spe_data->program_name);
  if (pthread_join(spe_data->thread, NULL)) {
    deprintf(2, "[PS3->SPU] Failed joining the thread: %s\n", spe_data->program_name);
    SDL_SetError("[PS3->SPU] Failed joining the thread");
    return -1;
  }

  return 0;
}

/* Create SPE context and load program */
int SPE_Boot(_THIS, spu_data_t * spe_data)
{
  /* Create SPE context */
  deprintf(2, "[PS3->SPU] Create SPE Context: %s\n", spe_data->program_name);
  spe_data->ctx = spe_context_create(0, NULL);
  if (spe_data->ctx == NULL) {
    deprintf(2, "[PS3->SPU] Failed creating SPE context: %s\n", spe_data->program_name);
    SDL_SetError("[PS3->SPU] Failed creating SPE context");
    return -1;
  }

  /* Load SPE object into SPE local store */
  deprintf(2, "[PS3->SPU] Load Program into SPE: %s\n", spe_data->program_name);
  if (spe_program_load(spe_data->ctx, &spe_data->program)) {
    deprintf(2, "[PS3->SPU] Failed loading program into SPE context: %s\n", spe_data->program_name);
    SDL_SetError
        ("[PS3->SPU] Failed loading program into SPE context");
    return -1;
  }
  spe_data->booted = 1;
  deprintf(2, "[PS3->SPU] SPE boot successful\n");

  return 0;
}

/* (Stop and) shutdown the SPE */
int SPE_Shutdown(_THIS, spu_data_t * spe_data)
{
  if (spe_data->keepalive && spe_data->booted) {
    SPE_SendMsg(_this, spe_data, SPU_EXIT);
    SPE_Stop(_this, spe_data);
  }

  /* Destroy SPE context */
  deprintf(2, "[PS3->SPU] Destroy SPE context: %s\n", spe_data->program_name);
  if (spe_context_destroy(spe_data->ctx)) {
    deprintf(2, "[PS3->SPU] Failed destroying context: %s\n", spe_data->program_name);
    SDL_SetError("[PS3->SPU] Failed destroying context");
    return -1;
  }
  deprintf(2, "[PS3->SPU] SPE shutdown successful: %s\n", spe_data->program_name);
  return 0;
}

/* Send message to the SPE via mailboxe */
int SPE_SendMsg(_THIS, spu_data_t * spe_data, unsigned int msg)
{
  deprintf(2, "[PS3->SPU] Sending message %u to %s\n", msg, spe_data->program_name);
  /* Send one message, block until message was sent */
  unsigned int spe_in_mbox_msgs[1];
  spe_in_mbox_msgs[0] = msg;
  int in_mbox_write = spe_in_mbox_write(spe_data->ctx, spe_in_mbox_msgs, 1, SPE_MBOX_ALL_BLOCKING);

  if (1 > in_mbox_write) {
    deprintf(2, "[PS3->SPU] No message could be written to %s\n", spe_data->program_name);
    SDL_SetError("[PS3->SPU] No message could be written");
    return -1;
  }
  return 0;
}


/* Read 1 message from SPE, block until at least 1 message was received */
int SPE_WaitForMsg(_THIS, spu_data_t * spe_data, unsigned int msg)
{
  deprintf(2, "[PS3->SPU] Waiting for message from %s\n", spe_data->program_name);
  unsigned int out_messages[1];
  while (!spe_out_mbox_status(spe_data->ctx));
  int mbox_read = spe_out_mbox_read(spe_data->ctx, out_messages, 1);
  deprintf(2, "[PS3->SPU] Got message from %s, message was %u\n", spe_data->program_name, out_messages[0]);
  if (out_messages[0] == msg)
    return 0;
  else
    return -1;
}

/* Re-runnable invocation of the spe_context_run call */
void SPE_RunContext(void *thread_argp)
{ 
  /* argp is the pointer to argument to be passed to the SPE program */
  spu_data_t *args = (spu_data_t *) thread_argp;
  deprintf(3, "[PS3->SPU] void* argp=0x%x\n", (unsigned int)args->argp);
  
  /* Run it.. */
  deprintf(2, "[PS3->SPU] Run SPE program: %s\n", args->program_name);
  if (spe_context_run
      (args->ctx, &args->entry, 0, (void *)args->argp, NULL,
       NULL) < 0) {
    deprintf(2, "[PS3->SPU] Failed running SPE context: %s\n", args->program_name);
    SDL_SetError("[PS3->SPU] Failed running SPE context: %s", args->program_name);
    exit(1);
  }

  pthread_exit(NULL);
}

/* vi: set ts=4 sw=4 expandtab: */
