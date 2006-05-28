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

/**
 * \file SDL_pixels.h
 *
 * Header for the enumerated pixel format definitions
 */

#ifndef _SDL_pixels_h
#define _SDL_pixels_h

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
/* *INDENT-OFF* */
extern "C" {
/* *INDENT-ON* */
#endif

enum
{                               /* Pixel type */
    SDL_PixelType_Unknown,
    SDL_PixelType_Index1,
    SDL_PixelType_Index4,
    SDL_PixelType_Index8,
    SDL_PixelType_Packed8,
    SDL_PixelType_Packed16,
    SDL_PixelType_Packed32,
    SDL_PixelType_ArrayU8,
    SDL_PixelType_ArrayU16,
    SDL_PixelType_ArrayU32,
    SDL_PixelType_ArrayF16,
    SDL_PixelType_ArrayF32,
};

enum
{                               /* bitmap pixel order, high bit -> low bit */
    SDL_BitmapOrder_None,
    SDL_BitmapOrder_4321,
    SDL_BitmapOrder_1234,
};
enum
{                               /* packed component order, high bit -> low bit */
    SDL_PackedOrder_None,
    SDL_PackedOrder_XRGB,
    SDL_PackedOrder_RGBX,
    SDL_PackedOrder_ARGB,
    SDL_PackedOrder_RGBA,
    SDL_PackedOrder_XBGR,
    SDL_PackedOrder_BGRX,
    SDL_PackedOrder_ABGR,
    SDL_PackedOrder_BGRA,
};
enum
{                               /* array component order, low byte -> high byte */
    SDL_ArrayOrder_None,
    SDL_ArrayOrder_RGB,
    SDL_ArrayOrder_RGBA,
    SDL_ArrayOrder_ARGB,
    SDL_ArrayOrder_BGR,
    SDL_ArrayOrder_BGRA,
    SDL_ArrayOrder_ABGR,
};

enum
{                               /* Packed component layout */
    SDL_PackedLayout_None,
    SDL_PackedLayout_332,
    SDL_PackedLayout_4444,
    SDL_PackedLayout_1555,
    SDL_PackedLayout_5551,
    SDL_PackedLayout_565,
    SDL_PackedLayout_8888,
    SDL_PackedLayout_2101010,
    SDL_PackedLayout_1010102,
};

#define SDL_DEFINE_PIXELFORMAT(type, order, layout, bits, bytes) \
    ((1 << 31) | ((type) << 24) | ((order) << 20) | ((layout) << 16) | \
     ((bits) << 8) | ((bytes) << 0))

#define SDL_PIXELTYPE(X)	(((X) >> 24) & 0x0F)
#define SDL_PIXELORDER(X)	(((X) >> 20) & 0x0F)
#define SDL_PIXELLAYOUT(X)	(((X) >> 16) & 0x0F)
#define SDL_BITSPERPIXEL(X)	(((X) >> 8) & 0xFF)
#define SDL_BYTESPERPIXEL(X)	(((X) >> 0) & 0xFF)

enum
{
    SDL_PixelFormat_Unknown,
    SDL_PixelFormat_Index1LSB =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Index1, SDL_BitmapOrder_1234, 0,
                                1, 0),
    SDL_PixelFormat_Index1MSB =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Index1, SDL_BitmapOrder_4321, 0,
                                1, 0),
    SDL_PixelFormat_Index4LSB =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Index4, SDL_BitmapOrder_1234, 0,
                                2, 0),
    SDL_PixelFormat_Index4MSB =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Index4, SDL_BitmapOrder_4321, 0,
                                2, 0),
    SDL_PixelFormat_Index8 =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Index8, 0, 0, 8, 1),
    SDL_PixelFormat_RGB332 =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Packed8, SDL_PackedOrder_XRGB,
                                SDL_PackedLayout_332, 8, 1),
    SDL_PixelFormat_RGB444 =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Packed16, SDL_PackedOrder_XRGB,
                                SDL_PackedLayout_4444, 12, 2),
    SDL_PixelFormat_RGB555 =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Packed16, SDL_PackedOrder_XRGB,
                                SDL_PackedLayout_1555, 15, 2),
    SDL_PixelFormat_ARGB4444 =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Packed16, SDL_PackedOrder_ARGB,
                                SDL_PackedLayout_4444, 16, 2),
    SDL_PixelFormat_ARGB1555 =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Packed16, SDL_PackedOrder_ARGB,
                                SDL_PackedLayout_1555, 16, 2),
    SDL_PixelFormat_RGB565 =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Packed16, SDL_PackedOrder_XRGB,
                                SDL_PackedLayout_565, 16, 2),
    SDL_PixelFormat_RGB24 =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_ArrayU8, SDL_ArrayOrder_RGB, 0,
                                24, 3),
    SDL_PixelFormat_BGR24 =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_ArrayU8, SDL_ArrayOrder_BGR, 0,
                                24, 3),
    SDL_PixelFormat_RGB888 =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Packed32, SDL_PackedOrder_XRGB,
                                SDL_PackedLayout_8888, 24, 4),
    SDL_PixelFormat_BGR888 =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Packed32, SDL_PackedOrder_XBGR,
                                SDL_PackedLayout_8888, 24, 4),
    SDL_PixelFormat_ARGB8888 =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Packed32, SDL_PackedOrder_ARGB,
                                SDL_PackedLayout_8888, 32, 4),
    SDL_PixelFormat_RGBA8888 =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Packed32, SDL_PackedOrder_RGBA,
                                SDL_PackedLayout_8888, 32, 4),
    SDL_PixelFormat_ABGR8888 =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Packed32, SDL_PackedOrder_ABGR,
                                SDL_PackedLayout_8888, 32, 4),
    SDL_PixelFormat_BGRA8888 =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Packed32, SDL_PackedOrder_BGRA,
                                SDL_PackedLayout_8888, 32, 4),
    SDL_PixelFormat_ARGB2101010 =
        SDL_DEFINE_PIXELFORMAT (SDL_PixelType_Packed32, SDL_PackedOrder_ARGB,
                                SDL_PackedLayout_2101010, 32, 4),
};

typedef struct SDL_Color
{
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 unused;
} SDL_Color;
#define SDL_Colour SDL_Color

typedef struct SDL_Palette
{
    int ncolors;
    SDL_Color *colors;
} SDL_Palette;

/* Everything in the pixel format structure is read-only */
typedef struct SDL_PixelFormat
{
    SDL_Palette *palette;
    Uint8 BitsPerPixel;
    Uint8 BytesPerPixel;
    Uint8 Rloss;
    Uint8 Gloss;
    Uint8 Bloss;
    Uint8 Aloss;
    Uint8 Rshift;
    Uint8 Gshift;
    Uint8 Bshift;
    Uint8 Ashift;
    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;
    Uint32 Amask;

    /* RGB color key information */
    Uint32 colorkey;
    /* Alpha value information (per-surface alpha) */
    Uint8 alpha;
} SDL_PixelFormat;

/*
 * Convert one of the enumerated formats above to a bpp and RGBA masks.
 * Returns SDL_TRUE, or SDL_FALSE if the conversion wasn't possible.
 */
extern DECLSPEC SDL_bool SDL_PixelFormatEnumToMasks (Uint32 format, int *bpp,
                                                     Uint32 * Rmask,
                                                     Uint32 * Gmask,
                                                     Uint32 * Bmask,
                                                     Uint32 * Amask);

/*
 * Convert a bpp and RGBA masks to one of the enumerated formats above.
 * Returns SDL_PixelFormat_Unknown if the conversion wasn't possible.
 */
extern DECLSPEC Uint32 SDL_MasksToPixelFormatEnum (int bpp, Uint32 Rmask,
                                                   Uint32 Gmask, Uint32 Bmask,
                                                   Uint32 Amask);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
/* *INDENT-OFF* */
}
/* *INDENT-ON* */
#endif
#include "close_code.h"

#endif /* _SDL_pixels_h */

/* vi: set ts=4 sw=4 expandtab: */
