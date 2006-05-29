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
 * \file SDL_video.h
 *
 * Header file for access to the SDL raw framebuffer window
 */

#ifndef _SDL_video_h
#define _SDL_video_h

#include "SDL_stdinc.h"
#include "SDL_error.h"
#include "SDL_pixels.h"
#include "SDL_rwops.h"

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
/* *INDENT-OFF* */
extern "C" {
/* *INDENT-ON* */
#endif

/* Transparency definitions: These define alpha as the opacity of a surface */
#define SDL_ALPHA_OPAQUE 255
#define SDL_ALPHA_TRANSPARENT 0

/* Useful data types */
typedef struct SDL_Rect
{
    Sint16 x, y;
    Uint16 w, h;
} SDL_Rect;

/* This structure should be treated as read-only, except for 'pixels',
   which, if not NULL, contains the raw pixel data for the surface.
*/
typedef struct SDL_Surface
{
    Uint32 flags;               /* Read-only */
    SDL_PixelFormat *format;    /* Read-only */
    int w, h;                   /* Read-only */
    Uint16 pitch;               /* Read-only */
    void *pixels;               /* Read-write */
    int offset;                 /* Private */

    /* Hardware-specific surface info */
    struct private_hwdata *hwdata;

    /* clipping information */
    SDL_Rect clip_rect;         /* Read-only */
    Uint32 unused1;             /* for binary compatibility */

    /* Allow recursive locks */
    Uint32 locked;              /* Private */

    /* info for fast blit mapping to other surfaces */
    struct SDL_BlitMap *map;    /* Private */

    /* format version, bumped at every change to invalidate blit maps */
    unsigned int format_version;        /* Private */

    /* Reference count -- used when freeing surface */
    int refcount;               /* Read-mostly */
} SDL_Surface;

/* The most common video overlay formats.
   For an explanation of these pixel formats, see:
	http://www.webartz.com/fourcc/indexyuv.htm

   For information on the relationship between color spaces, see:
   http://www.neuro.sfc.keio.ac.jp/~aly/polygon/info/color-space-faq.html
 */
#define SDL_YV12_OVERLAY  0x32315659    /* Planar mode: Y + V + U  (3 planes) */
#define SDL_IYUV_OVERLAY  0x56555949    /* Planar mode: Y + U + V  (3 planes) */
#define SDL_YUY2_OVERLAY  0x32595559    /* Packed mode: Y0+U0+Y1+V0 (1 plane) */
#define SDL_UYVY_OVERLAY  0x59565955    /* Packed mode: U0+Y0+V0+Y1 (1 plane) */
#define SDL_YVYU_OVERLAY  0x55595659    /* Packed mode: Y0+V0+Y1+U0 (1 plane) */

/* The YUV hardware video overlay */
typedef struct SDL_Overlay
{
    Uint32 format;              /* Read-only */
    int w, h;                   /* Read-only */
    int planes;                 /* Read-only */
    Uint16 *pitches;            /* Read-only */
    Uint8 **pixels;             /* Read-write */

    /* Hardware-specific surface info */
    struct private_yuvhwfuncs *hwfuncs;
    struct private_yuvhwdata *hwdata;

    /* Special flags */
    Uint32 hw_overlay:1;        /* Flag: This overlay hardware accelerated? */
    Uint32 UnusedBits:31;
} SDL_Overlay;

/* Evaluates to true if the surface needs to be locked before access */
#define SDL_MUSTLOCK(surface)	\
  (surface->offset ||		\
  ((surface->flags & (SDL_HWSURFACE|SDL_RLEACCEL)) != 0))

/* typedef for private surface blitting functions */
typedef int (*SDL_blit) (struct SDL_Surface * src, SDL_Rect * srcrect,
                         struct SDL_Surface * dst, SDL_Rect * dstrect);


/* Useful for determining the video hardware capabilities */
typedef struct SDL_VideoInfo
{
    Uint32 hw_available:1;      /* Flag: Can you create hardware surfaces? */
    Uint32 wm_available:1;      /* Flag: Can you talk to a window manager? */
    Uint32 UnusedBits1:6;
    Uint32 UnusedBits2:1;
    Uint32 blit_hw:1;           /* Flag: Accelerated blits HW --> HW */
    Uint32 blit_hw_CC:1;        /* Flag: Accelerated blits with Colorkey */
    Uint32 blit_hw_A:1;         /* Flag: Accelerated blits with Alpha */
    Uint32 blit_sw:1;           /* Flag: Accelerated blits SW --> HW */
    Uint32 blit_sw_CC:1;        /* Flag: Accelerated blits with Colorkey */
    Uint32 blit_sw_A:1;         /* Flag: Accelerated blits with Alpha */
    Uint32 blit_fill:1;         /* Flag: Accelerated color fill */
    Uint32 UnusedBits3:16;
    Uint32 video_mem;           /* The total amount of video memory (in K) */
} SDL_VideoInfo;

/**
 * \struct SDL_DisplayMode
 *
 * \brief  The structure that defines a display mode
 *
 * \sa SDL_GetNumDisplayModes()
 * \sa SDL_GetDisplayMode()
 * \sa SDL_GetDesktopDisplayMode()
 * \sa SDL_GetCurrentDisplayMode()
 * \sa SDL_GetClosestDisplayMode()
 * \sa SDL_SetDisplayMode()
 */
typedef struct
{
    Uint32 format;              /**< pixel format */
    int w;                      /**< width */
    int h;                      /**< height */
    int refresh_rate;           /**< refresh rate (or zero for unspecified) */
} SDL_DisplayMode;

/**
 * \typedef SDL_WindowID
 *
 * \brief The type used to identify a window
 *
 * \sa SDL_CreateWindow()
 * \sa SDL_CreateWindowFrom()
 * \sa SDL_DestroyWindow()
 * \sa SDL_GetWindowData()
 * \sa SDL_GetWindowFlags()
 * \sa SDL_GetWindowGrab()
 * \sa SDL_GetWindowPosition()
 * \sa SDL_GetWindowSize()
 * \sa SDL_GetWindowTitle()
 * \sa SDL_HideWindow()
 * \sa SDL_MaximizeWindow()
 * \sa SDL_MinimizeWindow()
 * \sa SDL_RaiseWindow()
 * \sa SDL_RestoreWindow()
 * \sa SDL_SetWindowData()
 * \sa SDL_SetWindowGrab()
 * \sa SDL_SetWindowIcon()
 * \sa SDL_SetWindowPosition()
 * \sa SDL_SetWindowSize()
 * \sa SDL_SetWindowTitle()
 * \sa SDL_ShowWindow()
 */
typedef Uint32 SDL_WindowID;

/**
 * \enum SDL_WindowFlags
 *
 * \brief The flags on a window
 */
typedef enum
{
    SDL_WINDOW_FULLSCREEN = 0x00000001,         /**< fullscreen window, implies borderless */
    SDL_WINDOW_BORDERLESS = 0x00000002,         /**< no window decoration */
    SDL_WINDOW_SHOWN = 0x00000004,              /**< window is visible */
    SDL_WINDOW_OPENGL = 0x00000008,             /**< window usable with OpenGL context */
    SDL_WINDOW_RESIZABLE = 0x00000010,          /**< window can be resized */
    SDL_WINDOW_MAXIMIZED = 0x00000020,          /**< maximized */
    SDL_WINDOW_MINIMIZED = 0x00000040,          /**< minimized */
    SDL_WINDOW_INPUT_GRABBED = 0x00000080,      /**< window has grabbed input focus */
    SDL_WINDOW_KEYBOARD_FOCUS = 0x00000100,     /**< window has keyboard focus */
    SDL_WINDOW_MOUSE_FOCUS = 0x00000200,        /**< window has mouse focus */
} SDL_WindowFlags;

/**
 * \enum SDL_WindowEventID
 *
 * \brief Event subtype for window events
 */
typedef enum
{
    SDL_WINDOWEVENT_NONE,               /**< Never used */
    SDL_WINDOWEVENT_SHOWN,              /**< Window has been shown */
    SDL_WINDOWEVENT_HIDDEN,             /**< Window has been hidden */
    SDL_WINDOWEVENT_MOVED,              /**< Window has been moved to data1,data2 */
    SDL_WINDOWEVENT_RESIZED,            /**< Window size changed to data1xdata2 */
    SDL_WINDOWEVENT_MINIMIZED,          /**< Window has been minimized */
    SDL_WINDOWEVENT_MAXIMIZED,          /**< Window has been maximized */
    SDL_WINDOWEVENT_RESTORED,           /**< Window has been restored to normal size and position */
    SDL_WINDOWEVENT_ENTER,              /**< The window has gained mouse focus */
    SDL_WINDOWEVENT_LEAVE,              /**< The window has lost mouse focus */
    SDL_WINDOWEVENT_FOCUS_GAINED,       /**< The window has gained keyboard focus */
    SDL_WINDOWEVENT_FOCUS_LOST,         /**< The window has lost keyboard focus */
} SDL_WindowEventID;

/**
 * \enum SDL_GLattr
 *
 * \brief OpenGL configuration attributes
 */
typedef enum
{
    SDL_GL_RED_SIZE,
    SDL_GL_GREEN_SIZE,
    SDL_GL_BLUE_SIZE,
    SDL_GL_ALPHA_SIZE,
    SDL_GL_BUFFER_SIZE,
    SDL_GL_DOUBLEBUFFER,
    SDL_GL_DEPTH_SIZE,
    SDL_GL_STENCIL_SIZE,
    SDL_GL_ACCUM_RED_SIZE,
    SDL_GL_ACCUM_GREEN_SIZE,
    SDL_GL_ACCUM_BLUE_SIZE,
    SDL_GL_ACCUM_ALPHA_SIZE,
    SDL_GL_STEREO,
    SDL_GL_MULTISAMPLEBUFFERS,
    SDL_GL_MULTISAMPLESAMPLES,
    SDL_GL_ACCELERATED_VISUAL,
    SDL_GL_SWAP_CONTROL
} SDL_GLattr;

/* These are the currently supported flags for the SDL_surface */
#define SDL_SWSURFACE	0x00000000      /* Surface is in system memory */
#define SDL_HWSURFACE	0x00000001      /* Surface is in video memory */
/* Available for SDL_CreateWindowSurface() */
#define SDL_ANYFORMAT	0x10000000      /* Allow any video depth/pixel-format */
#define SDL_HWPALETTE	0x20000000      /* Surface has exclusive palette */
#define SDL_DOUBLEBUF	0x40000000      /* Set up double-buffered surface */
/* Used internally (read-only) */
#define SDL_HWACCEL	0x00000100      /* Blit uses hardware acceleration */
#define SDL_SRCCOLORKEY	0x00001000      /* Blit uses a source color key */
#define SDL_RLEACCELOK	0x00002000      /* Private flag */
#define SDL_RLEACCEL	0x00004000      /* Surface is RLE encoded */
#define SDL_SRCALPHA	0x00010000      /* Blit uses source alpha blending */
#define SDL_PREALLOC	0x00100000      /* Surface uses preallocated memory */
#define SDL_SCREEN_SURFACE 0x01000000   /* Surface is a window screen surface */
#define SDL_SHADOW_SURFACE 0x02000000   /* Surface is a window shadow surface */

/* Function prototypes */

/**
 * \fn int SDL_GetNumVideoDrivers(void)
 *
 * \brief Get the number of video drivers compiled into SDL
 *
 * \sa SDL_GetVideoDriver()
 */
extern DECLSPEC int SDLCALL SDL_GetNumVideoDrivers(void);

/**
 * \fn const char *SDL_GetVideoDriver(int index)
 *
 * \brief Get the name of a built in video driver.
 *
 * \note The video drivers are presented in the order in which they are
 * normally checked during initialization.
 *
 * \sa SDL_GetNumVideoDrivers()
 */
extern DECLSPEC const char *SDLCALL SDL_GetVideoDriver(int index);

/**
 * \fn int SDL_VideoInit(const char *driver_name, Uint32 flags)
 *
 * \brief Initialize the video subsystem, optionally specifying a video driver.
 *
 * \param driver_name Initialize a specific driver by name, or NULL for the default video driver.
 * \param flags FIXME: Still needed?
 *
 * \return 0 on success, -1 on error
 *
 * This function initializes the video subsystem; setting up a connection
 * to the window manager, etc, and determines the available display modes
 * and pixel formats, but does not initialize a window or graphics mode.
 *
 * \sa SDL_VideoQuit()
 */
extern DECLSPEC int SDLCALL SDL_VideoInit(const char *driver_name,
                                          Uint32 flags);

/**
 * \fn void SDL_VideoQuit(void)
 *
 * \brief Shuts down the video subsystem.
 *
 * This function closes all windows, and restores the original video mode.
 *
 * \sa SDL_VideoInit()
 */
extern DECLSPEC void SDLCALL SDL_VideoQuit(void);

/**
 * \fn const char *SDL_GetCurrentVideoDriver(void)
 *
 * \brief Returns the name of the currently initialized video driver.
 *
 * \return The name of the current video driver or NULL if no driver
 *         has been initialized
 *
 * \sa SDL_GetNumVideoDrivers()
 * \sa SDL_GetVideoDriver()
 */
extern DECLSPEC const char *SDLCALL SDL_GetCurrentVideoDriver(void);

/**
 * \fn const SDL_VideoInfo *SDL_GetVideoInfo(void)
 *
 * \brief Returns information about the currently initialized video driver.
 *
 * \return A read-only pointer to information about the video hardware,
 *         or NULL if no video driver has been initialized.
 */
extern DECLSPEC const SDL_VideoInfo *SDLCALL SDL_GetVideoInfo(void);

/**
 * \fn int SDL_GetNumVideoDisplays(void)
 *
 * \brief Returns the number of available video displays.
 *
 * \sa SDL_SelectVideoDisplay()
 */
extern DECLSPEC int SDLCALL SDL_GetNumVideoDisplays(void);

/**
 * \fn int SDL_SelectVideoDisplay(int index)
 *
 * \brief Set the index of the currently selected display.
 *
 * \note You can query the currently selected display by passing an index of -1.
 *
 * \sa SDL_GetNumVideoDisplays()
 */
extern DECLSPEC int SDLCALL SDL_SelectVideoDisplay(int index);

/**
 * \fn int SDL_GetNumDisplayModes(void)
 *
 * \brief Returns the number of available display modes for the current display.
 *
 * \sa SDL_GetDisplayMode()
 */
extern DECLSPEC int SDLCALL SDL_GetNumDisplayModes(void);

/**
 * \fn const SDL_DisplayMode *SDL_GetDisplayMode(int index)
 *
 * \brief Retrieve information about a specific display mode.
 *
 * \note The display modes are sorted in this priority:
 *       \li bits per pixel -> more colors to fewer colors
 *       \li width -> largest to smallest
 *       \li height -> largest to smallest
 *       \li refresh rate -> highest to lowest
 *
 * \sa SDL_GetNumDisplayModes()
 */
extern DECLSPEC const SDL_DisplayMode *SDLCALL SDL_GetDisplayMode(int index);

/**
 * \fn const SDL_DisplayMode *SDL_GetDesktopDisplayMode(void)
 *
 * \brief Retrieve information about the desktop display mode for the current display.
 */
extern DECLSPEC const SDL_DisplayMode *SDLCALL
SDL_GetDesktopDisplayMode(void);

/**
 * \fn const SDL_DisplayMode *SDL_GetCurrentDisplayMode(void)
 *
 * \brief Retrieve information about the current display mode.
 */
extern DECLSPEC const SDL_DisplayMode *SDLCALL
SDL_GetCurrentDisplayMode(void);

/**
 * \fn SDL_DisplayMode *SDL_GetClosestDisplayMode(const SDL_DisplayMode *mode, SDL_DisplayMode *closest)
 *
 * \brief Get the closest match to the requested display mode.
 *
 * \param mode The desired display mode
 * \param closest A pointer to a display mode to be filled in with the closest match of the available display modes.
 *
 * \return The passed in value 'closest', or NULL if no matching video mode was available.
 *
 * The available display modes are scanned, and 'closest' is filled in with the closest mode matching the requested mode and returned.  The mode format and refresh_rate default to the desktop mode if they are 0.  The modes are scanned with size being first priority, format being second priority, and finally checking the refresh_rate.  If all the available modes are too small, then NULL is returned.
 *
 * \sa SDL_GetNumDisplayModes()
 * \sa SDL_GetDisplayMode()
 */
extern DECLSPEC SDL_DisplayMode *SDLCALL SDL_GetClosestDisplayMode(const
                                                                   SDL_DisplayMode
                                                                   * mode,
                                                                   SDL_DisplayMode
                                                                   * closest);

/**
 * \fn int SDL_SetDisplayMode(const SDL_DisplayMode *mode)
 *
 * \brief Set up the closest available mode on the current display.
 *
 * \param mode The desired display mode
 *
 * \return 0 on success, or -1 if setting the display mode failed.
 */
extern DECLSPEC int SDLCALL SDL_SetDisplayMode(const SDL_DisplayMode * mode);

/**
 * \fn SDL_WindowID SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags)
 *
 * \brief Create a window with the specified position, dimensions, and flags.
 *
 * \param title The title of the window
 * \param x The x position of the window
 * \param y The y position of the window
 * \param w The width of the window
 * \param h The height of the window
 * \param flags The flags for the window
 *
 * \return The id of the window created, or zero if window creation failed.
 *
 * \note Setting the position to -1, -1, indicates any position is fine.
 *
 * \sa SDL_DestroyWindow()
 */
extern DECLSPEC SDL_WindowID SDLCALL SDL_CreateWindow(const char *title,
                                                      int x, int y, int w,
                                                      int h, Uint32 flags);

/**
 * \fn SDL_WindowID SDL_CreateWindowFrom(void *data)
 *
 * \brief Create an SDL window struct from an existing native window.
 *
 * \param data A pointer to driver-dependent window creation data
 *
 * \return The id of the window created, or zero if window creation failed.
 *
 * \warning This function is NOT SUPPORTED, use at your own risk!
 *
 * \sa SDL_DestroyWindow()
 */
extern DECLSPEC SDL_WindowID SDLCALL SDL_CreateWindowFrom(void *data);

/**
 * \fn Uint32 SDL_GetWindowFlags(SDL_WindowID windowID)
 *
 * \brief Get the window flags.
 */
extern DECLSPEC Uint32 SDLCALL SDL_GetWindowFlags(SDL_WindowID windowID);

/**
 * \fn void SDL_SetWindowTitle(SDL_WindowID windowID, const char *title)
 *
 * \brief Set the title of the window, in UTF-8 format.
 *
 * \sa SDL_GetWindowTitle()
 */
extern DECLSPEC void SDLCALL SDL_SetWindowTitle(SDL_WindowID windowID,
                                                const char *title);

/**
 * \fn const char *SDL_GetWindowTitle(SDL_WindowID windowID)
 *
 * \brief Get the title of the window, in UTF-8 format.
 *
 * \sa SDL_SetWindowTitle()
 */
extern DECLSPEC const char *SDLCALL SDL_GetWindowTitle(SDL_WindowID windowID);

/**
 * \fn void SDL_SetWindowIcon(SDL_Surface *icon)
 *
 * \brief Set the icon of the window.
 *
 * \param icon The icon for the window
 *
 * FIXME: The icon needs to be set before the window is first shown.  Should some icon representation be part of the window creation data?
 */
extern DECLSPEC void SDLCALL SDL_SetWindowIcon(SDL_Surface * icon);

/**
 * \fn void SDL_SetWindowData(SDL_WindowID windowID, void *userdata)
 *
 * \brief Associate an arbitrary pointer with the window.
 *
 * \sa SDL_GetWindowData()
 */
extern DECLSPEC void SDLCALL SDL_SetWindowData(SDL_WindowID windowID,
                                               void *userdata);

/**
 * \fn void *SDL_GetWindowData(SDL_WindowID windowID)
 *
 * \brief Retrieve the data pointer associated with the window.
 *
 * \sa SDL_SetWindowData()
 */
extern DECLSPEC void *SDLCALL SDL_GetWindowData(SDL_WindowID windowID);

/**
 * \fn void SDL_SetWindowPosition(SDL_WindowID windowID, int x, int y)
 *
 * \brief Set the position of the window.
 *
 * \sa SDL_GetWindowPosition()
 */
extern DECLSPEC void SDLCALL SDL_SetWindowPosition(SDL_WindowID windowID,
                                                   int x, int y);

/**
 * \fn void SDL_GetWindowPosition(SDL_WindowID windowID, int *x, int *y)
 *
 * \brief Get the position of the window.
 *
 * \sa SDL_SetWindowPosition()
 */
extern DECLSPEC void SDLCALL SDL_GetWindowPosition(SDL_WindowID windowID,
                                                   int *x, int *y);

/**
 * \fn void SDL_SetWindowSize(SDL_WindowID windowID, int w, int w)
 *
 * \brief Set the size of the window's client area.
 *
 * \note You can't change the size of a fullscreen window, it automatically
 * matches the size of the display mode.
 *
 * \sa SDL_GetWindowSize()
 */
extern DECLSPEC void SDLCALL SDL_SetWindowSize(SDL_WindowID windowID, int w,
                                               int h);

/**
 * \fn void SDL_GetWindowSize(SDL_WindowID windowID, int *w, int *w)
 *
 * \brief Get the size of the window's client area.
 *
 * \sa SDL_SetWindowSize()
 */
extern DECLSPEC void SDLCALL SDL_GetWindowSize(SDL_WindowID windowID, int *w,
                                               int *h);

/**
 * \fn void SDL_ShowWindow(SDL_WindowID windowID)
 *
 * \brief Show the window
 *
 * \sa SDL_HideWindow()
 */
extern DECLSPEC void SDLCALL SDL_ShowWindow(SDL_WindowID windowID);

/**
 * \fn void SDL_HideWindow(SDL_WindowID windowID)
 *
 * \brief Hide the window
 *
 * \sa SDL_ShowWindow()
 */
extern DECLSPEC void SDLCALL SDL_HideWindow(SDL_WindowID windowID);

/**
 * \fn void SDL_RaiseWindow(SDL_WindowID windowID)
 *
 * \brief Raise the window so it's above other windows.
 */
extern DECLSPEC void SDLCALL SDL_RaiseWindow(SDL_WindowID windowID);

/**
 * \fn void SDL_MaximizeWindow(SDL_WindowID windowID)
 *
 * \brief Make the window as large as possible.
 *
 * \sa SDL_RestoreWindow()
 */
extern DECLSPEC void SDLCALL SDL_MaximizeWindow(SDL_WindowID windowID);

/**
 * \fn void SDL_MinimizeWindow(SDL_WindowID windowID)
 *
 * \brief Minimize the window to an iconic representation.
 *
 * \sa SDL_RestoreWindow()
 */
extern DECLSPEC void SDLCALL SDL_MinimizeWindow(SDL_WindowID windowID);

/**
 * \fn void SDL_RestoreWindow(SDL_WindowID windowID)
 *
 * \brief Restore the size and position of a minimized or maximized window.
 *
 * \sa SDL_MaximizeWindow()
 * \sa SDL_MinimizeWindow()
 */
extern DECLSPEC void SDLCALL SDL_RestoreWindow(SDL_WindowID windowID);

/**
 * \fn void SDL_SetWindowGrab(SDL_WindowID windowID, int mode)
 *
 * \brief Set the window's input grab mode.
 *
 * \param mode This is 1 to grab input, and 0 to release input.
 *
 * \sa SDL_GrabMode
 * \sa SDL_GetWindowGrab()
 */
extern DECLSPEC void SDLCALL SDL_SetWindowGrab(SDL_WindowID windowID,
                                               int mode);

/**
 * \fn int SDL_GetWindowGrab(SDL_WindowID windowID)
 *
 * \brief Get the window's input grab mode.
 *
 * \return This returns 1 if input is grabbed, and 0 otherwise.
 *
 * \sa SDL_GrabMode
 * \sa SDL_SetWindowGrab()
 */
extern DECLSPEC int SDLCALL SDL_GetWindowGrab(SDL_WindowID windowID);

/**
 * \fn void SDL_DestroyWindow(SDL_WindowID windowID)
 *
 * \brief Destroy a window.
 */
extern DECLSPEC void SDLCALL SDL_DestroyWindow(SDL_WindowID windowID);

/**
 * \fn SDL_Surface *SDL_CreateWindowSurface (SDL_WindowID windowID, Uint32 format, Uint32 flags)
 *
 * \brief Create an SDL_Surface representing the drawing area of the window.
 */
extern DECLSPEC SDL_Surface *SDLCALL SDL_CreateWindowSurface(SDL_WindowID
                                                             windowID,
                                                             Uint32 format,
                                                             Uint32 flags);

/*
 * Makes sure the given list of rectangles is updated on the given screen.
 * If 'x', 'y', 'w' and 'h' are all 0, SDL_UpdateRect will update the entire
 * screen.
 * These functions should not be called while 'screen' is locked.
 */
extern DECLSPEC void SDLCALL SDL_UpdateRects
    (SDL_Surface * screen, int numrects, SDL_Rect * rects);
extern DECLSPEC void SDLCALL SDL_UpdateRect
    (SDL_Surface * screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h);

/*
 * On hardware that supports double-buffering, this function sets up a flip
 * and returns.  The hardware will wait for vertical retrace, and then swap
 * video buffers before the next video surface blit or lock will return.
 * On hardware that doesn not support double-buffering, this is equivalent
 * to calling SDL_UpdateRect(screen, 0, 0, 0, 0);
 * The SDL_DOUBLEBUF flag must have been passed to SDL_SetVideoMode() when
 * setting the video mode for this function to perform hardware flipping.
 * This function returns 0 if successful, or -1 if there was an error.
 */
extern DECLSPEC int SDLCALL SDL_Flip(SDL_Surface * screen);

/*
 * Set the gamma correction for each of the color channels.
 * The gamma values range (approximately) between 0.1 and 10.0
 * 
 * If this function isn't supported directly by the hardware, it will
 * be emulated using gamma ramps, if available.  If successful, this
 * function returns 0, otherwise it returns -1.
 */
extern DECLSPEC int SDLCALL SDL_SetGamma(float red, float green, float blue);

/*
 * Set the gamma translation table for the red, green, and blue channels
 * of the video hardware.  Each table is an array of 256 16-bit quantities,
 * representing a mapping between the input and output for that channel.
 * The input is the index into the array, and the output is the 16-bit
 * gamma value at that index, scaled to the output color precision.
 * 
 * You may pass NULL for any of the channels to leave it unchanged.
 * If the call succeeds, it will return 0.  If the display driver or
 * hardware does not support gamma translation, or otherwise fails,
 * this function will return -1.
 */
extern DECLSPEC int SDLCALL SDL_SetGammaRamp(const Uint16 * red,
                                             const Uint16 * green,
                                             const Uint16 * blue);

/*
 * Retrieve the current values of the gamma translation tables.
 * 
 * You must pass in valid pointers to arrays of 256 16-bit quantities.
 * Any of the pointers may be NULL to ignore that channel.
 * If the call succeeds, it will return 0.  If the display driver or
 * hardware does not support gamma translation, or otherwise fails,
 * this function will return -1.
 */
extern DECLSPEC int SDLCALL SDL_GetGammaRamp(Uint16 * red, Uint16 * green,
                                             Uint16 * blue);

/*
 * Sets a portion of the colormap for the given 8-bit surface.  If 'surface'
 * is not a palettized surface, this function does nothing, returning 0.
 * If all of the colors were set as passed to SDL_SetColors(), it will
 * return 1.  If not all the color entries were set exactly as given,
 * it will return 0, and you should look at the surface palette to
 * determine the actual color palette.
 *
 * When 'surface' is the surface associated with the current display, the
 * display colormap will be updated with the requested colors.  If 
 * SDL_HWPALETTE was set in SDL_SetVideoMode() flags, SDL_SetColors()
 * will always return 1, and the palette is guaranteed to be set the way
 * you desire, even if the window colormap has to be warped or run under
 * emulation.
 */
extern DECLSPEC int SDLCALL SDL_SetColors(SDL_Surface * surface,
                                          SDL_Color * colors, int firstcolor,
                                          int ncolors);

/*
 * Maps an RGB triple to an opaque pixel value for a given pixel format
 */
extern DECLSPEC Uint32 SDLCALL SDL_MapRGB
    (SDL_PixelFormat * format, Uint8 r, Uint8 g, Uint8 b);

/*
 * Maps an RGBA quadruple to a pixel value for a given pixel format
 */
extern DECLSPEC Uint32 SDLCALL SDL_MapRGBA(SDL_PixelFormat * format,
                                           Uint8 r, Uint8 g, Uint8 b,
                                           Uint8 a);

/*
 * Maps a pixel value into the RGB components for a given pixel format
 */
extern DECLSPEC void SDLCALL SDL_GetRGB(Uint32 pixel, SDL_PixelFormat * fmt,
                                        Uint8 * r, Uint8 * g, Uint8 * b);

/*
 * Maps a pixel value into the RGBA components for a given pixel format
 */
extern DECLSPEC void SDLCALL SDL_GetRGBA(Uint32 pixel, SDL_PixelFormat * fmt,
                                         Uint8 * r, Uint8 * g, Uint8 * b,
                                         Uint8 * a);

/*
 * Allocate and free an RGB surface (must be called after SDL_SetVideoMode)
 * If the depth is 4 or 8 bits, an empty palette is allocated for the surface.
 * If the depth is greater than 8 bits, the pixel format is set using the
 * flags '[RGB]mask'.
 * If the function runs out of memory, it will return NULL.
 *
 * The 'flags' tell what kind of surface to create.
 * SDL_SWSURFACE means that the surface should be created in system memory.
 * SDL_HWSURFACE means that the surface should be created in video memory,
 * with the same format as the display surface.  This is useful for surfaces
 * that will not change much, to take advantage of hardware acceleration
 * when being blitted to the display surface.
 * SDL_ASYNCBLIT means that SDL will try to perform asynchronous blits with
 * this surface, but you must always lock it before accessing the pixels.
 * SDL will wait for current blits to finish before returning from the lock.
 * SDL_SRCCOLORKEY indicates that the surface will be used for colorkey blits.
 * If the hardware supports acceleration of colorkey blits between
 * two surfaces in video memory, SDL will try to place the surface in
 * video memory. If this isn't possible or if there is no hardware
 * acceleration available, the surface will be placed in system memory.
 * SDL_SRCALPHA means that the surface will be used for alpha blits and 
 * if the hardware supports hardware acceleration of alpha blits between
 * two surfaces in video memory, to place the surface in video memory
 * if possible, otherwise it will be placed in system memory.
 * If the surface is created in video memory, blits will be _much_ faster,
 * but the surface format must be identical to the video surface format,
 * and the only way to access the pixels member of the surface is to use
 * the SDL_LockSurface() and SDL_UnlockSurface() calls.
 * If the requested surface actually resides in video memory, SDL_HWSURFACE
 * will be set in the flags member of the returned surface.  If for some
 * reason the surface could not be placed in video memory, it will not have
 * the SDL_HWSURFACE flag set, and will be created in system memory instead.
 */
#define SDL_AllocSurface    SDL_CreateRGBSurface
extern DECLSPEC SDL_Surface *SDLCALL SDL_CreateRGBSurface
    (Uint32 flags, int width, int height, int depth,
     Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
extern DECLSPEC SDL_Surface *SDLCALL SDL_CreateRGBSurfaceFrom(void *pixels,
                                                              int width,
                                                              int height,
                                                              int depth,
                                                              int pitch,
                                                              Uint32 Rmask,
                                                              Uint32 Gmask,
                                                              Uint32 Bmask,
                                                              Uint32 Amask);
extern DECLSPEC void SDLCALL SDL_FreeSurface(SDL_Surface * surface);

/*
 * SDL_LockSurface() sets up a surface for directly accessing the pixels.
 * Between calls to SDL_LockSurface()/SDL_UnlockSurface(), you can write
 * to and read from 'surface->pixels', using the pixel format stored in 
 * 'surface->format'.  Once you are done accessing the surface, you should 
 * use SDL_UnlockSurface() to release it.
 *
 * Not all surfaces require locking.  If SDL_MUSTLOCK(surface) evaluates
 * to 0, then you can read and write to the surface at any time, and the
 * pixel format of the surface will not change.  In particular, if the
 * SDL_HWSURFACE flag is not given when calling SDL_SetVideoMode(), you
 * will not need to lock the display surface before accessing it.
 * 
 * No operating system or library calls should be made between lock/unlock
 * pairs, as critical system locks may be held during this time.
 *
 * SDL_LockSurface() returns 0, or -1 if the surface couldn't be locked.
 */
extern DECLSPEC int SDLCALL SDL_LockSurface(SDL_Surface * surface);
extern DECLSPEC void SDLCALL SDL_UnlockSurface(SDL_Surface * surface);

/*
 * Load a surface from a seekable SDL data source (memory or file.)
 * If 'freesrc' is non-zero, the source will be closed after being read.
 * Returns the new surface, or NULL if there was an error.
 * The new surface should be freed with SDL_FreeSurface().
 */
extern DECLSPEC SDL_Surface *SDLCALL SDL_LoadBMP_RW(SDL_RWops * src,
                                                    int freesrc);

/* Convenience macro -- load a surface from a file */
#define SDL_LoadBMP(file)	SDL_LoadBMP_RW(SDL_RWFromFile(file, "rb"), 1)

/*
 * Save a surface to a seekable SDL data source (memory or file.)
 * If 'freedst' is non-zero, the source will be closed after being written.
 * Returns 0 if successful or -1 if there was an error.
 */
extern DECLSPEC int SDLCALL SDL_SaveBMP_RW
    (SDL_Surface * surface, SDL_RWops * dst, int freedst);

/* Convenience macro -- save a surface to a file */
#define SDL_SaveBMP(surface, file) \
		SDL_SaveBMP_RW(surface, SDL_RWFromFile(file, "wb"), 1)

/*
 * Sets the color key (transparent pixel) in a blittable surface.
 * If 'flag' is SDL_SRCCOLORKEY (optionally OR'd with SDL_RLEACCEL), 
 * 'key' will be the transparent pixel in the source image of a blit.
 * SDL_RLEACCEL requests RLE acceleration for the surface if present,
 * and removes RLE acceleration if absent.
 * If 'flag' is 0, this function clears any current color key.
 * This function returns 0, or -1 if there was an error.
 */
extern DECLSPEC int SDLCALL SDL_SetColorKey
    (SDL_Surface * surface, Uint32 flag, Uint32 key);

/*
 * This function sets the alpha value for the entire surface, as opposed to
 * using the alpha component of each pixel. This value measures the range
 * of transparency of the surface, 0 being completely transparent to 255
 * being completely opaque. An 'alpha' value of 255 causes blits to be
 * opaque, the source pixels copied to the destination (the default). Note
 * that per-surface alpha can be combined with colorkey transparency.
 *
 * If 'flag' is 0, alpha blending is disabled for the surface.
 * If 'flag' is SDL_SRCALPHA, alpha blending is enabled for the surface.
 * OR:ing the flag with SDL_RLEACCEL requests RLE acceleration for the
 * surface; if SDL_RLEACCEL is not specified, the RLE accel will be removed.
 *
 * The 'alpha' parameter is ignored for surfaces that have an alpha channel.
 */
extern DECLSPEC int SDLCALL SDL_SetAlpha(SDL_Surface * surface, Uint32 flag,
                                         Uint8 alpha);

/*
 * Sets the clipping rectangle for the destination surface in a blit.
 *
 * If the clip rectangle is NULL, clipping will be disabled.
 * If the clip rectangle doesn't intersect the surface, the function will
 * return SDL_FALSE and blits will be completely clipped.  Otherwise the
 * function returns SDL_TRUE and blits to the surface will be clipped to
 * the intersection of the surface area and the clipping rectangle.
 *
 * Note that blits are automatically clipped to the edges of the source
 * and destination surfaces.
 */
extern DECLSPEC SDL_bool SDLCALL SDL_SetClipRect(SDL_Surface * surface,
                                                 const SDL_Rect * rect);

/*
 * Gets the clipping rectangle for the destination surface in a blit.
 * 'rect' must be a pointer to a valid rectangle which will be filled
 * with the correct values.
 */
extern DECLSPEC void SDLCALL SDL_GetClipRect(SDL_Surface * surface,
                                             SDL_Rect * rect);

/*
 * Creates a new surface of the specified format, and then copies and maps 
 * the given surface to it so the blit of the converted surface will be as 
 * fast as possible.  If this function fails, it returns NULL.
 *
 * The 'flags' parameter is passed to SDL_CreateRGBSurface() and has those 
 * semantics.  You can also pass SDL_RLEACCEL in the flags parameter and
 * SDL will try to RLE accelerate colorkey and alpha blits in the resulting
 * surface.
 *
 * This function is used internally by SDL_DisplayFormat().
 */
extern DECLSPEC SDL_Surface *SDLCALL SDL_ConvertSurface
    (SDL_Surface * src, SDL_PixelFormat * fmt, Uint32 flags);

/*
 * This performs a fast blit from the source surface to the destination
 * surface.  It assumes that the source and destination rectangles are
 * the same size.  If either 'srcrect' or 'dstrect' are NULL, the entire
 * surface (src or dst) is copied.  The final blit rectangles are saved
 * in 'srcrect' and 'dstrect' after all clipping is performed.
 * If the blit is successful, it returns 0, otherwise it returns -1.
 *
 * The blit function should not be called on a locked surface.
 *
 * The blit semantics for surfaces with and without alpha and colorkey
 * are defined as follows:
 *
 * RGBA->RGB:
 *     SDL_SRCALPHA set:
 * 	alpha-blend (using alpha-channel).
 * 	SDL_SRCCOLORKEY ignored.
 *     SDL_SRCALPHA not set:
 * 	copy RGB.
 * 	if SDL_SRCCOLORKEY set, only copy the pixels matching the
 * 	RGB values of the source colour key, ignoring alpha in the
 * 	comparison.
 * 
 * RGB->RGBA:
 *     SDL_SRCALPHA set:
 * 	alpha-blend (using the source per-surface alpha value);
 * 	set destination alpha to opaque.
 *     SDL_SRCALPHA not set:
 * 	copy RGB, set destination alpha to source per-surface alpha value.
 *     both:
 * 	if SDL_SRCCOLORKEY set, only copy the pixels matching the
 * 	source colour key.
 * 
 * RGBA->RGBA:
 *     SDL_SRCALPHA set:
 * 	alpha-blend (using the source alpha channel) the RGB values;
 * 	leave destination alpha untouched. [Note: is this correct?]
 * 	SDL_SRCCOLORKEY ignored.
 *     SDL_SRCALPHA not set:
 * 	copy all of RGBA to the destination.
 * 	if SDL_SRCCOLORKEY set, only copy the pixels matching the
 * 	RGB values of the source colour key, ignoring alpha in the
 * 	comparison.
 * 
 * RGB->RGB: 
 *     SDL_SRCALPHA set:
 * 	alpha-blend (using the source per-surface alpha value).
 *     SDL_SRCALPHA not set:
 * 	copy RGB.
 *     both:
 * 	if SDL_SRCCOLORKEY set, only copy the pixels matching the
 * 	source colour key.
 *
 * If either of the surfaces were in video memory, and the blit returns -2,
 * the video memory was lost, so it should be reloaded with artwork and 
 * re-blitted:
	while ( SDL_BlitSurface(image, imgrect, screen, dstrect) == -2 ) {
		while ( SDL_LockSurface(image) < 0 )
			Sleep(10);
		-- Write image pixels to image->pixels --
		SDL_UnlockSurface(image);
	}
 * This happens under DirectX 5.0 when the system switches away from your
 * fullscreen application.  The lock will also fail until you have access
 * to the video memory again.
 */
/* You should call SDL_BlitSurface() unless you know exactly how SDL
   blitting works internally and how to use the other blit functions.
*/
#define SDL_BlitSurface SDL_UpperBlit

/* This is the public blit function, SDL_BlitSurface(), and it performs
   rectangle validation and clipping before passing it to SDL_LowerBlit()
*/
extern DECLSPEC int SDLCALL SDL_UpperBlit
    (SDL_Surface * src, SDL_Rect * srcrect,
     SDL_Surface * dst, SDL_Rect * dstrect);
/* This is a semi-private blit function and it performs low-level surface
   blitting only.
*/
extern DECLSPEC int SDLCALL SDL_LowerBlit
    (SDL_Surface * src, SDL_Rect * srcrect,
     SDL_Surface * dst, SDL_Rect * dstrect);

/*
 * This function performs a fast fill of the given rectangle with 'color'
 * The given rectangle is clipped to the destination surface clip area
 * and the final fill rectangle is saved in the passed in pointer.
 * If 'dstrect' is NULL, the whole surface will be filled with 'color'
 * The color should be a pixel of the format used by the surface, and 
 * can be generated by the SDL_MapRGB() function.
 * This function returns 0 on success, or -1 on error.
 */
extern DECLSPEC int SDLCALL SDL_FillRect
    (SDL_Surface * dst, SDL_Rect * dstrect, Uint32 color);

/* 
 * This function takes a surface and copies it to a new surface of the
 * pixel format and colors of the video framebuffer, suitable for fast
 * blitting onto the display surface.  It calls SDL_ConvertSurface()
 *
 * If you want to take advantage of hardware colorkey or alpha blit
 * acceleration, you should set the colorkey and alpha value before
 * calling this function.
 *
 * If the conversion fails or runs out of memory, it returns NULL
 */
extern DECLSPEC SDL_Surface *SDLCALL SDL_DisplayFormat(SDL_Surface * surface);

/* 
 * This function takes a surface and copies it to a new surface of the
 * pixel format and colors of the video framebuffer (if possible),
 * suitable for fast alpha blitting onto the display surface.
 * The new surface will always have an alpha channel.
 *
 * If you want to take advantage of hardware colorkey or alpha blit
 * acceleration, you should set the colorkey and alpha value before
 * calling this function.
 *
 * If the conversion fails or runs out of memory, it returns NULL
 */
extern DECLSPEC SDL_Surface *SDLCALL SDL_DisplayFormatAlpha(SDL_Surface *
                                                            surface);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* YUV video surface overlay functions                                       */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* This function creates a video output overlay
   Calling the returned surface an overlay is something of a misnomer because
   the contents of the display surface underneath the area where the overlay
   is shown is undefined - it may be overwritten with the converted YUV data.
*/
extern DECLSPEC SDL_Overlay *SDLCALL SDL_CreateYUVOverlay(int width,
                                                          int height,
                                                          Uint32 format,
                                                          SDL_Surface *
                                                          display);

/* Lock an overlay for direct access, and unlock it when you are done */
extern DECLSPEC int SDLCALL SDL_LockYUVOverlay(SDL_Overlay * overlay);
extern DECLSPEC void SDLCALL SDL_UnlockYUVOverlay(SDL_Overlay * overlay);

/* Blit a video overlay to the display surface.
   The contents of the video surface underneath the blit destination are
   not defined.  
   The width and height of the destination rectangle may be different from
   that of the overlay, but currently only 2x scaling is supported.
*/
extern DECLSPEC int SDLCALL SDL_DisplayYUVOverlay(SDL_Overlay * overlay,
                                                  SDL_Rect * dstrect);

/* Free a video overlay */
extern DECLSPEC void SDLCALL SDL_FreeYUVOverlay(SDL_Overlay * overlay);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* OpenGL support functions.                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * Dynamically load an OpenGL library, or the default one if path is NULL
 *
 * If you do this, you need to retrieve all of the GL functions used in
 * your program from the dynamic library using SDL_GL_GetProcAddress().
 */
extern DECLSPEC int SDLCALL SDL_GL_LoadLibrary(const char *path);

/*
 * Get the address of a GL function
 */
extern DECLSPEC void *SDLCALL SDL_GL_GetProcAddress(const char *proc);

/*
 * Set an attribute of the OpenGL subsystem before window creation.
 */
extern DECLSPEC int SDLCALL SDL_GL_SetAttribute(SDL_GLattr attr, int value);

/*
 * Get an attribute of the OpenGL subsystem from the windowing
 * interface, such as glX. This is of course different from getting
 * the values from SDL's internal OpenGL subsystem, which only
 * stores the values you request before initialization.
 *
 * Developers should track the values they pass into SDL_GL_SetAttribute
 * themselves if they want to retrieve these values.
 */
extern DECLSPEC int SDLCALL SDL_GL_GetAttribute(SDL_GLattr attr, int *value);

/*
 * Swap the OpenGL buffers, if double-buffering is supported.
 */
extern DECLSPEC void SDLCALL SDL_GL_SwapBuffers(void);

/* Not in public API at the moment - do not use! */
extern DECLSPEC int SDLCALL SDL_SoftStretch(SDL_Surface * src,
                                            SDL_Rect * srcrect,
                                            SDL_Surface * dst,
                                            SDL_Rect * dstrect);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
/* *INDENT-OFF* */
}
/* *INDENT-ON* */
#endif
#include "close_code.h"

#endif /* _SDL_video_h */

/* vi: set ts=4 sw=4 expandtab: */
