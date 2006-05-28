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

/* Gamma correction support */

#ifdef HAVE_MATH_H
#include <math.h>               /* Used for calculating gamma ramps */
#else
/* Math routines from uClibc: http://www.uclibc.org */
#include "math_private.h"
#include "e_sqrt.h"
#include "e_pow.h"
#include "e_log.h"
#define pow(x, y)	__ieee754_pow(x, y)
#define log(x)		__ieee754_log(x)
#endif

#include "SDL_sysvideo.h"


static void
CalculateGammaRamp (float gamma, Uint16 * ramp)
{
    int i;

    /* 0.0 gamma is all black */
    if (gamma <= 0.0f) {
        for (i = 0; i < 256; ++i) {
            ramp[i] = 0;
        }
        return;
    } else
        /* 1.0 gamma is identity */
    if (gamma >= 1.0f) {
        for (i = 0; i < 256; ++i) {
            ramp[i] = (i << 8) | i;
        }
        return;
    } else
        /* Calculate a real gamma ramp */
    {
        int value;
        gamma = 1.0f / gamma;
        for (i = 0; i < 256; ++i) {
            value = (int) (pow ((double) i / 256.0, gamma) * 65535.0 + 0.5);
            if (value > 65535) {
                value = 65535;
            }
            ramp[i] = (Uint16) value;
        }
    }
}
static void
CalculateGammaFromRamp (float *gamma, Uint16 * ramp)
{
    /* The following is adapted from a post by Garrett Bass on OpenGL
       Gamedev list, March 4, 2000.
     */
    float sum = 0.0f;
    int i, count = 0;

    *gamma = 1.0;
    for (i = 1; i < 256; ++i) {
        if ((ramp[i] != 0) && (ramp[i] != 65535)) {
            double B = (double) i / 256.0;
            double A = ramp[i] / 65535.0;
            sum += (float) (log (A) / log (B));
            count++;
        }
    }
    if (count && sum > 0.0f) {
        *gamma = 1.0f / (sum / count);
    }
}

int
SDL_SetGamma (float red, float green, float blue)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice ();
    int succeeded;

    succeeded = -1;
    /* Prefer using SetGammaRamp(), as it's more flexible */
    {
        Uint16 ramp[3][256];

        CalculateGammaRamp (red, ramp[0]);
        CalculateGammaRamp (green, ramp[1]);
        CalculateGammaRamp (blue, ramp[2]);
        succeeded = SDL_SetGammaRamp (ramp[0], ramp[1], ramp[2]);
    }
    if ((succeeded < 0) && _this->SetGamma) {
        SDL_ClearError ();
        succeeded = _this->SetGamma (_this, red, green, blue);
    }
    return succeeded;
}

/* Calculating the gamma by integrating the gamma ramps isn't exact,
   so this function isn't officially supported.
*/
int
SDL_GetGamma (float *red, float *green, float *blue)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice ();
    int succeeded;

    succeeded = -1;
    /* Prefer using GetGammaRamp(), as it's more flexible */
    {
        Uint16 ramp[3][256];

        succeeded = SDL_GetGammaRamp (ramp[0], ramp[1], ramp[2]);
        if (succeeded >= 0) {
            CalculateGammaFromRamp (red, ramp[0]);
            CalculateGammaFromRamp (green, ramp[1]);
            CalculateGammaFromRamp (blue, ramp[2]);
        }
    }
    if ((succeeded < 0) && _this->GetGamma) {
        SDL_ClearError ();
        succeeded = _this->GetGamma (_this, red, green, blue);
    }
    return succeeded;
}

int
SDL_SetGammaRamp (const Uint16 * red, const Uint16 * green,
                  const Uint16 * blue)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice ();
    int succeeded;
    SDL_Surface *screen = SDL_PublicSurface;

    /* Verify the screen parameter */
    if (!screen) {
        SDL_SetError ("No video mode has been set");
        return -1;
    }

    /* Lazily allocate the gamma tables */
    if (!SDL_CurrentWindow.gamma) {
        SDL_GetGammaRamp (0, 0, 0);
    }

    /* Fill the gamma table with the new values */
    if (red) {
        SDL_memcpy (&SDL_CurrentWindow.gamma[0 * 256], red,
                    256 * sizeof (*SDL_CurrentWindow.gamma));
    }
    if (green) {
        SDL_memcpy (&SDL_CurrentWindow.gamma[1 * 256], green,
                    256 * sizeof (*SDL_CurrentWindow.gamma));
    }
    if (blue) {
        SDL_memcpy (&SDL_CurrentWindow.gamma[2 * 256], blue,
                    256 * sizeof (*SDL_CurrentWindow.gamma));
    }

    /* Try to set the gamma ramp in the driver */
    succeeded = -1;
    if (_this->SetGammaRamp) {
        succeeded = _this->SetGammaRamp (_this, SDL_CurrentWindow.gamma);
    } else {
        SDL_SetError ("Gamma ramp manipulation not supported");
    }
    return succeeded;
}

int
SDL_GetGammaRamp (Uint16 * red, Uint16 * green, Uint16 * blue)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice ();

    /* Lazily allocate the gamma table */
    if (!SDL_CurrentWindow.gamma) {
        SDL_CurrentWindow.gamma =
            SDL_malloc (3 * 256 * sizeof (*SDL_CurrentWindow.gamma));
        if (!SDL_CurrentWindow.gamma) {
            SDL_OutOfMemory ();
            return -1;
        }
        if (_this->GetGammaRamp) {
            /* Get the real hardware gamma */
            _this->GetGammaRamp (_this, SDL_CurrentWindow.gamma);
        } else {
            /* Assume an identity gamma */
            int i;
            for (i = 0; i < 256; ++i) {
                SDL_CurrentWindow.gamma[0 * 256 + i] = (i << 8) | i;
                SDL_CurrentWindow.gamma[1 * 256 + i] = (i << 8) | i;
                SDL_CurrentWindow.gamma[2 * 256 + i] = (i << 8) | i;
            }
        }
    }

    /* Just copy from our internal table */
    if (red) {
        SDL_memcpy (red, &SDL_CurrentWindow.gamma[0 * 256],
                    256 * sizeof (*red));
    }
    if (green) {
        SDL_memcpy (green, &SDL_CurrentWindow.gamma[1 * 256],
                    256 * sizeof (*green));
    }
    if (blue) {
        SDL_memcpy (blue, &SDL_CurrentWindow.gamma[2 * 256],
                    256 * sizeof (*blue));
    }
    return 0;
}

/* vi: set ts=4 sw=4 expandtab: */
