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
*/
#include "SDL_config.h"

/* SDL Nintendo DS video driver implementation
 * based on dummy driver:
 * Initial work by Ryan C. Gordon (icculus@icculus.org). A good portion
 *  of this was cut-and-pasted from Stephane Peter's work in the AAlib
 *  SDL video driver.  Renamed to "DUMMY" by Sam Lantinga.
 */

#include <stdio.h>
#include <stdlib.h>
#include <nds.h>
#include <nds/arm9/sprite.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/video.h>

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"

#include "SDL_ndsvideo.h"
#include "SDL_ndsevents_c.h"
#include "SDL_ndsrender_c.h"

#define NDSVID_DRIVER_NAME "nds"

/* Initialization/Query functions */
static int NDS_VideoInit(_THIS);
static int NDS_SetDisplayMode(_THIS, SDL_DisplayMode * mode);
static void NDS_VideoQuit(_THIS);

/* NDS sprite-related functions */

#define SPRITE_DMA_CHANNEL 3
#define SPRITE_ANGLE_MASK 0x01FF

void
NDS_OAM_Update(tOAM *oam)
{
    DC_FlushAll();
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL, oam->spriteBuffer, OAM,
                     SPRITE_COUNT * sizeof(SpriteEntry));
}

void
NDS_OAM_RotateSprite(SpriteRotation *spriteRotation, u16 angle)
{
    s16 s = SIN[angle & SPRITE_ANGLE_MASK] >> 4;
    s16 c = COS[angle & SPRITE_ANGLE_MASK] >> 4;

    spriteRotation->hdx = c;
    spriteRotation->hdy = s;
    spriteRotation->vdx = -s;
    spriteRotation->vdy = c;
}

void
NDS_OAM_Init(tOAM *oam)
{
    int i;
    for(i = 0; i < SPRITE_COUNT; i++) {
        oam->spriteBuffer[i].attribute[0] = ATTR0_DISABLED;
        oam->spriteBuffer[i].attribute[1] = 0;
        oam->spriteBuffer[i].attribute[2] = 0;
    }
    for(i = 0; i < MATRIX_COUNT; i++) {
        NDS_OAM_RotateSprite(&(oam->matrixBuffer[i]), 0);
    }
    swiWaitForVBlank();
    NDS_OAM_Update(oam);
}

void
NDS_OAM_HideSprite(SpriteEntry *spriteEntry)
{
    spriteEntry->isRotoscale = 0;
    spriteEntry->isHidden = 1;
}

void
NDS_OAM_ShowSprite(SpriteEntry *spriteEntry, int affine, int double_bound)
{
    if (affine) {
        spriteEntry->isRotoscale = 1;
        spriteEntry->rsDouble = double_bound;
    } else {
        spriteEntry->isHidden = 0;
    }
}


/* SDL NDS driver bootstrap functions */
static int
NDS_Available(void)
{
    /*const char *envr = SDL_getenv("SDL_VIDEODRIVER");*/
    /*printf("NDS_Available()\n"); */
    return (1);
}

static void
NDS_DeleteDevice(SDL_VideoDevice * device)
{
    SDL_free(device);
}

static SDL_VideoDevice *
NDS_CreateDevice(int devindex)
{
    SDL_VideoDevice *device;
    /*printf("NDS_CreateDevice(%d)\n", devindex); */

printf("+NDS_CreateDevice\n");
    /* Initialize all variables that we clean on shutdown */
    device = (SDL_VideoDevice *) SDL_calloc(1, sizeof(SDL_VideoDevice));
    if (!device) {
        SDL_OutOfMemory();
        if (device) {
            SDL_free(device);
        }
        return (0);
    }

    /* Set the function pointers */
    device->VideoInit = NDS_VideoInit;
    device->VideoQuit = NDS_VideoQuit;
    device->SetDisplayMode = NDS_SetDisplayMode;
    device->PumpEvents = NDS_PumpEvents;

    device->num_displays = 2;   /* DS = dual screens */

    device->free = NDS_DeleteDevice;

printf("-NDS_CreateDevice\n");
    return device;
}

VideoBootStrap NDS_bootstrap = {
    NDSVID_DRIVER_NAME, "SDL NDS video driver",
    NDS_Available, NDS_CreateDevice
};

int
NDS_VideoInit(_THIS)
{
    SDL_DisplayMode mode;
    int i;

printf("+NDS_VideoInit\n");
    /* simple 256x192x16x60 for now */
    mode.w = 256;
    mode.h = 192;
    mode.format = SDL_PIXELFORMAT_ABGR1555;
    mode.refresh_rate = 60;
    mode.driverdata = NULL;

    SDL_AddBasicVideoDisplay(&mode);
    SDL_AddRenderDriver(0, &NDS_RenderDriver);
    /*SDL_AddBasicVideoDisplay(&mode); two screens, same mode. uncomment later
    SDL_AddRenderDriver(1, &NDS_RenderDriver);*/

    SDL_zero(mode);
    SDL_AddDisplayMode(0, &mode);

    /* hackish stuff to get things up and running for now, and for a console */
    powerON(POWER_ALL_2D);    irqInit();
    irqEnable(IRQ_VBLANK);
    NDS_SetDisplayMode(_this, &mode);
printf("-NDS_VideoInit\n");
    return 0;
}

static int
NDS_SetDisplayMode(_THIS, SDL_DisplayMode * mode)
{
printf("+NDS_SetDisplayMode\n");
    /* right now this function is just hard-coded for 256x192 ABGR1555 */
    videoSetMode(MODE_5_2D |
        DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE |
        DISPLAY_SPR_1D_LAYOUT | DISPLAY_SPR_ACTIVE); /* display on main core */
    videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE); /* debug text on sub */
    vramSetMainBanks(VRAM_A_MAIN_BG_0x06000000, VRAM_B_MAIN_BG_0x06020000,
                     VRAM_C_SUB_BG_0x06200000,  VRAM_D_MAIN_BG_0x06040000);
    vramSetBankE(VRAM_E_MAIN_SPRITE);
    /* set up console for debug text 'n stuff */
    SUB_BG0_CR = BG_MAP_BASE(31);
    BG_PALETTE_SUB[255] = RGB15(31, 31, 31);
   /* debugging purposes, uncomment this later.  then remove it & add 2screen.
    consoleInitDefault((u16 *) SCREEN_BASE_BLOCK_SUB(31),
                       (u16 *) CHAR_BASE_BLOCK_SUB(0), 16);*/
printf("-NDS_SetDisplayMode\n");
    return 0;
}

void
NDS_VideoQuit(_THIS)
{
printf("+NDS_VideoQuit\n");
    videoSetMode(DISPLAY_SCREEN_OFF);
    videoSetModeSub(DISPLAY_SCREEN_OFF);
    vramSetMainBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);
printf("-NDS_VideoQuit\n");
}

/* vi: set ts=4 sw=4 expandtab: */
