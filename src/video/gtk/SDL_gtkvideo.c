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

/*
 * GTK+ SDL video driver implementation; this is a little like a pared-down
 *  version of the X11 driver. You almost certainly do NOT want this target
 *  on a desktop machine. This was written for the One Laptop Per Child
 *  project so they wouldn't need to use the SDL_WINDOWID hack with the X11
 *  driver and compete for the event queue.
 *
 * Initial work by Ryan C. Gordon (icculus@icculus.org). A good portion
 *  of this was cut-and-pasted from the dummy video target just to have a
 *  starting point for the bare minimum to fill in, and some was lifted from
 *  the x11 target.
 */

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"

#include "SDL_gtkvideo.h"
#include "SDL_gtkevents_c.h"
#include "SDL_gtkmouse_c.h"

#define GTKPLUSVID_DRIVER_NAME "gtk"

/* Initialization/Query functions */
static int GTKPLUS_VideoInit(_THIS, SDL_PixelFormat *vformat);
static SDL_Rect **GTKPLUS_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags);
static SDL_Surface *GTKPLUS_SetVideoMode(_THIS, SDL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int GTKPLUS_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors);
static void GTKPLUS_VideoQuit(_THIS);

/* Hardware surface functions */
static int GTKPLUS_AllocHWSurface(_THIS, SDL_Surface *surface);
static int GTKPLUS_LockHWSurface(_THIS, SDL_Surface *surface);
static void GTKPLUS_UnlockHWSurface(_THIS, SDL_Surface *surface);
static void GTKPLUS_FreeHWSurface(_THIS, SDL_Surface *surface);
static void GTKPLUS_SetCaption(_THIS, const char *title, const char *icon);

/* etc. */
static void GTKPLUS_UpdateRects(_THIS, int numrects, SDL_Rect *rects);

/* GTKPLUS driver bootstrap functions */

static int do_gtk_init(void)
{
    static int initted = 0;
    if (!initted) {   /* !!! FIXME: I can't see a way to deinit gtk... */
        int tmpargc = 0;
        char *args[] = { NULL, NULL };
        char **tmpargv = args;
        initted = (gtk_init_check(&tmpargc, &tmpargv));
    }
    return initted;
}


static int GTKPLUS_Available(void)
{
    return 1;  /* !!! FIXME */
}

static void GTKPLUS_DeleteDevice(SDL_VideoDevice *device)
{
	SDL_free(device->hidden);
	SDL_free(device);
}

static SDL_VideoDevice *GTKPLUS_CreateDevice(int devindex)
{
	SDL_VideoDevice *device;

	/* Initialize all variables that we clean on shutdown */
	device = (SDL_VideoDevice *)SDL_malloc(sizeof(SDL_VideoDevice));
	if ( device ) {
		SDL_memset(device, 0, (sizeof *device));
		device->hidden = (struct SDL_PrivateVideoData *)
				SDL_malloc((sizeof *device->hidden));
	}
	if ( (device == NULL) || (device->hidden == NULL) ) {
		SDL_OutOfMemory();
		if ( device ) {
			SDL_free(device);
		}
		return(0);
	}
	SDL_memset(device->hidden, 0, (sizeof *device->hidden));

    device->handles_any_size = 1;

	/* Set the function pointers */
	device->VideoInit = GTKPLUS_VideoInit;
	device->ListModes = GTKPLUS_ListModes;
	device->SetVideoMode = GTKPLUS_SetVideoMode;
	device->CreateYUVOverlay = NULL;
	device->SetColors = GTKPLUS_SetColors;
	device->UpdateRects = GTKPLUS_UpdateRects;
	device->VideoQuit = GTKPLUS_VideoQuit;
	device->AllocHWSurface = GTKPLUS_AllocHWSurface;
	device->CheckHWBlit = NULL;
	device->FillHWRect = NULL;
	device->SetHWColorKey = NULL;
	device->SetHWAlpha = NULL;
	device->LockHWSurface = GTKPLUS_LockHWSurface;
	device->UnlockHWSurface = GTKPLUS_UnlockHWSurface;
	device->FlipHWSurface = NULL;
	device->FreeHWSurface = GTKPLUS_FreeHWSurface;
	device->SetCaption = GTKPLUS_SetCaption;
	device->SetIcon = NULL;
	device->IconifyWindow = NULL;
	device->GrabInput = NULL;
	device->GetWMInfo = NULL;
	device->InitOSKeymap = GTKPLUS_InitOSKeymap;
	device->PumpEvents = GTKPLUS_PumpEvents;

	device->free = GTKPLUS_DeleteDevice;

	return device;
}

VideoBootStrap GTKPLUS_bootstrap = {
	GTKPLUSVID_DRIVER_NAME, "SDL GTK+ video driver",
	GTKPLUS_Available, GTKPLUS_CreateDevice
};


static int add_visual(_THIS, int depth, GdkVisualType vistype)
{
    GdkVisual *vi = gdk_visual_get_best_with_both(depth, vistype);
    if(vi != NULL) {
        g_object_ref(vi);
        this->hidden->visuals[this->hidden->nvisuals++] = vi;
    }
    return(this->hidden->nvisuals);
}

static int GTKPLUS_GetVideoModes(_THIS)
{
    const gint screen_w = gdk_screen_width();
    const gint screen_h = gdk_screen_height();
    int i, n;

    {
	/* It's interesting to note that if we allow 32 bit depths (on X11),
	   we get a visual with an alpha mask on composite servers.
        static int depth_list[] = { 32, 24, 16, 15, 8 };
	*/
        static int depth_list[] = { 24, 16, 15, 8 };
        int use_directcolor = 1;

        /* Search for the visuals in deepest-first order, so that the first
           will be the richest one */
        if ( SDL_getenv("SDL_VIDEO_GTK_NODIRECTCOLOR") ) {
                use_directcolor = 0;
        }
        this->hidden->nvisuals = 0;
        for ( i=0; i<SDL_arraysize(depth_list); ++i ) {
            if ( depth_list[i] > 8 ) {
                if ( use_directcolor ) {
                    add_visual(this, depth_list[i], GDK_VISUAL_DIRECT_COLOR);
                }
                add_visual(this, depth_list[i], GDK_VISUAL_TRUE_COLOR);
            } else {
                add_visual(this, depth_list[i], GDK_VISUAL_PSEUDO_COLOR);
                add_visual(this, depth_list[i], GDK_VISUAL_STATIC_COLOR);
            }
        }
        if ( this->hidden->nvisuals == 0 ) {
            SDL_SetError("Found no sufficiently capable GTK+ visuals");
            return -1;
        }
    }

    return 0;
}


int GTKPLUS_VideoInit(_THIS, SDL_PixelFormat *vformat)
{
    GdkVisual *sysvis = NULL;
    int i;

    if (!do_gtk_init()) {
        return -1;
    }

	/* Get the available video modes */
	if(GTKPLUS_GetVideoModes(this) < 0)
	    return -1;

	/* Determine the current screen size */
	this->info.current_w = gdk_screen_width();
	this->info.current_h = gdk_screen_height();

	/* Determine the default screen depth:
	   Use the default visual (or at least one with the same depth) */
	this->hidden->display_colormap = gdk_colormap_get_system();  /* !!! FIXME: refcount? */
    sysvis = gdk_visual_get_system();  /* !!! FIXME: refcount? */

    for(i = 0; i < this->hidden->nvisuals; i++) {
        if(this->hidden->visuals[i]->depth == sysvis->depth)
            break;
    }

	if(i == this->hidden->nvisuals) {
	    /* default visual was useless, take the deepest one instead */
	    i = 0;
	}

	this->hidden->visual = this->hidden->visuals[i];
	if ( this->hidden->visual == sysvis ) {  /* !!! FIXME: same pointer? */
	    this->hidden->colormap = this->hidden->display_colormap;
        g_object_ref(this->hidden->colormap);
	} else {
	    this->hidden->colormap = gdk_colormap_new(this->hidden->visual, FALSE);
	}

    // !!! FIXME: this is not a public GDK symbol!!
	vformat->BitsPerPixel = _gdk_windowing_get_bits_for_depth(
                                gdk_display_get_default(),
                                this->hidden->visuals[i]->depth);
	this->hidden->depth = vformat->BitsPerPixel;

	if ( vformat->BitsPerPixel > 8 ) {
		vformat->Rmask = this->hidden->visual->red_mask;
	  	vformat->Gmask = this->hidden->visual->green_mask;
	  	vformat->Bmask = this->hidden->visual->blue_mask;
	}
	if ( this->hidden->depth == 32 ) {
		vformat->Amask = (0xFFFFFFFF & ~(vformat->Rmask|vformat->Gmask|vformat->Bmask));
	}

#if 0
	/* Create the fullscreen and managed windows */
	create_aux_windows(this);

	/* Create the blank cursor */
	SDL_BlankCursor = this->CreateWMCursor(this, blank_cdata, blank_cmask,
					BLANK_CWIDTH, BLANK_CHEIGHT,
						BLANK_CHOTX, BLANK_CHOTY);

	/* Allow environment override of screensaver disable. */
	env = SDL_getenv("SDL_VIDEO_ALLOW_SCREENSAVER");
	this->hidden->allow_screensaver = ( (env && SDL_atoi(env)) ? 1 : 0 );
#endif

	/* We're done! */
	gdk_flush();  /* just in case. */

	/* Fill in some window manager capabilities */
	this->info.wm_available = 1;

	return(0);
}


static GdkVisual *find_visual(_THIS, int bpp)
{
    GdkDisplay *display = gdk_display_get_default();
    int i;
    for ( i = 0; i < this->hidden->nvisuals; i++ ) {
        const int videpth = this->hidden->visuals[i]->depth;
        // !!! FIXME: this is not a public GDK symbol!!
        const int depth = _gdk_windowing_get_bits_for_depth(display, videpth);
        if ( depth == bpp )
            break;
    }

    if ( i == this->hidden->nvisuals ) {
        SDL_SetError("No matching visual for requested depth");
        return NULL;  /* should never happen */
    }
    return this->hidden->visuals[i];
}


SDL_Rect **GTKPLUS_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags)
{
    if ((flags & SDL_OPENGL) == 0) {
        if (find_visual(this, format->BitsPerPixel) != NULL) {
       	    return (SDL_Rect **) -1;  /* !!! FIXME: maybe not right. */
        }
    }
    return NULL;  /* unsupported. */
}


SDL_Surface *GTKPLUS_SetVideoMode(_THIS, SDL_Surface *current,
				int width, int height, int bpp, Uint32 flags)
{
    Uint32 Amask = 0;
    int vis_change = 0;
    GtkWindow *win = NULL;
    GdkImage *img = NULL;
    GdkVisual *sysvis = gdk_visual_get_system();  /* !!! FIXME: refcount? */
    GdkVisual *vis = find_visual(this, bpp);
    if (vis == NULL) {
		return(NULL);
	}

    if (flags & SDL_OPENGL) {
		SDL_SetError("No OpenGL support in the GTK+ target");
		return(NULL);
	}

    /* These are the only flags we allow here... */
    flags &= /*SDL_FULLSCREEN |*/ SDL_RESIZABLE | SDL_NOFRAME | SDL_HWPALETTE;

	vis_change = (vis != this->hidden->visual);
	this->hidden->visual = vis;
	this->hidden->depth = vis->depth;

	/* Allocate the new pixel format for this video mode */
	if ( this->hidden->depth == 32 ) {
		Amask = (0xFFFFFFFF & ~(vis->red_mask|vis->green_mask|vis->blue_mask));
	} else {
		Amask = 0;
	}
	if ( ! SDL_ReallocFormat(current, bpp,
			vis->red_mask, vis->green_mask, vis->blue_mask, Amask) ) {
		return NULL;
	}

	/* Create the appropriate colormap */
	g_object_unref(this->hidden->colormap);
    this->hidden->colormap = NULL;

	if ( this->hidden->visual->type == GDK_VISUAL_PSEUDO_COLOR ) {
	    int ncolors;

	    /* Allocate the pixel flags */
	    ncolors = this->hidden->visual->colormap_size;

	    #if 0
        SDL_XPixels = SDL_malloc(ncolors * sizeof(int));
	    if(SDL_XPixels == NULL) {
            SDL_OutOfMemory();
            return -1;
	    }
	    SDL_memset(SDL_XPixels, 0, ncolors * sizeof(*SDL_XPixels));
        #endif

	    /* always allocate a private colormap on non-default visuals */
	    if ( this->hidden->visual != sysvis ) {
            flags |= SDL_HWPALETTE;
	    }
	    if ( flags & SDL_HWPALETTE ) {
            current->flags |= SDL_HWPALETTE;
            this->hidden->colormap = gdk_colormap_new(this->hidden->visual, TRUE);
	    } else {
            this->hidden->colormap = this->hidden->display_colormap;
            g_object_ref(this->hidden->colormap);
	    }
	} else if ( this->hidden->visual->type == GDK_VISUAL_DIRECT_COLOR ) {

	    /* Create a colormap which we can manipulate for gamma */
        this->hidden->colormap = gdk_colormap_new(this->hidden->visual, TRUE);
        gdk_flush();

	    /* Initialize the colormap to the identity mapping */
	    SDL_GetGammaRamp(0, 0, 0);
	    this->screen = current;
#if 0 // !!! FIXME
	    GTKPLUS_SetGammaRamp(this, this->gamma);
#endif
	    this->screen = NULL;
	} else {
	    /* Create a read-only colormap for our window */
        this->hidden->colormap = gdk_colormap_new(this->hidden->visual, FALSE);
	}

#if 0  // !!! FIXME
	/* Recreate the auxiliary windows, if needed (required for GL) */
	if ( vis_change )
	    create_aux_windows(this);

	if(current->flags & SDL_HWPALETTE) {
	    /* Since the full-screen window might have got a nonzero background
	       colour (0 is white on some displays), we should reset the
	       background to 0 here since that is what the user expects
	       with a private colormap */
	    XSetWindowBackground(SDL_Display, FSwindow, 0);
	    XClearWindow(SDL_Display, FSwindow);
	}

	/* resize the (possibly new) window manager window */
	if( !SDL_windowid ) {
	        X11_SetSizeHints(this, w, h, flags);
		window_w = w;
		window_h = h;
		XResizeWindow(SDL_Display, WMwindow, w, h);
	}
#endif

    if ( this->hidden->gdkimage ) {
        g_object_unref(this->hidden->gdkimage);
        this->hidden->gdkimage = NULL;
    }

    img = this->hidden->gdkimage = gdk_image_new(GDK_IMAGE_FASTEST,
                                           vis, width, height);
    if (img == NULL) {
		SDL_SetError("Couldn't allocate buffer for requested mode");
		return(NULL);
	}
    gdk_image_set_colormap(this->hidden->gdkimage, this->hidden->colormap);

	SDL_memset(img->mem, 0, height * img->bpl);

    if ( this->hidden->gtkwindow == NULL ) {
        this->hidden->gtkdrawingarea = gtk_drawing_area_new();
        if ( this->hidden->gtkdrawingarea == NULL ) {
		    SDL_SetError("Couldn't create drawing area for requested mode");
            g_object_unref(this->hidden->gdkimage);
            this->hidden->gdkimage = NULL;
		    return(NULL);
	    }

        this->hidden->gtkwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        if ( this->hidden->gtkwindow == NULL ) {
		    SDL_SetError("Couldn't create window for requested mode");
            g_object_unref(this->hidden->gdkimage);
            g_object_unref(this->hidden->gtkdrawingarea);
            this->hidden->gdkimage = NULL;
            this->hidden->gtkdrawingarea = NULL;
		    return(NULL);
	    }

        gtk_window_set_title(GTK_WINDOW(this->hidden->gtkwindow), "");
        gtk_widget_set_app_paintable(this->hidden->gtkwindow, TRUE);
        gtk_widget_set_app_paintable(this->hidden->gtkdrawingarea, TRUE);
        gtk_widget_set_double_buffered(this->hidden->gtkwindow, FALSE);
        gtk_widget_set_double_buffered(this->hidden->gtkdrawingarea, FALSE);

        GTKPLUS_ConnectSignals(this);

        gtk_container_add(GTK_CONTAINER(this->hidden->gtkwindow),
                          this->hidden->gtkdrawingarea);
    }

    win = GTK_WINDOW(this->hidden->gtkwindow);
    gtk_widget_set_colormap(this->hidden->gtkdrawingarea, this->hidden->colormap);

// !!! FIXME
#if 0
	/* Cache the window in the server, when possible */
	{
		Screen *xscreen;
		XSetWindowAttributes a;

		xscreen = ScreenOfDisplay(SDL_Display, SDL_Screen);
		a.backing_store = DoesBackingStore(xscreen);
		if ( a.backing_store != NotUseful ) {
			XChangeWindowAttributes(SDL_Display, SDL_Window,
			                        CWBackingStore, &a);
		}
	}

	/* Update the internal keyboard state */
	X11_SetKeyboardState(SDL_Display, NULL);

	/* When the window is first mapped, ignore non-modifier keys */
	{
		Uint8 *keys = SDL_GetKeyState(NULL);
		for ( i = 0; i < SDLK_LAST; ++i ) {
			switch (i) {
			    case SDLK_NUMLOCK:
			    case SDLK_CAPSLOCK:
			    case SDLK_LCTRL:
			    case SDLK_RCTRL:
			    case SDLK_LSHIFT:
			    case SDLK_RSHIFT:
			    case SDLK_LALT:
			    case SDLK_RALT:
			    case SDLK_LMETA:
			    case SDLK_RMETA:
			    case SDLK_MODE:
				break;
			    default:
				keys[i] = SDL_RELEASED;
				break;
			}
		}
	}

	/* Map them both and go fullscreen, if requested */
	if ( ! SDL_windowid ) {
		XMapWindow(SDL_Display, SDL_Window);
		XMapWindow(SDL_Display, WMwindow);
		X11_WaitMapped(this, WMwindow);
		if ( flags & SDL_FULLSCREEN ) {
			current->flags |= SDL_FULLSCREEN;
			X11_EnterFullScreen(this);
		} else {
			current->flags &= ~SDL_FULLSCREEN;
		}
	}
#endif

    if ((flags & SDL_FULLSCREEN) == 0) {
        gtk_window_unfullscreen(win);
    } else {
        gtk_window_fullscreen(win);
        flags &= ~SDL_RESIZABLE;
        flags |= SDL_NOFRAME;
    }

    gtk_window_set_resizable(win, (flags & SDL_RESIZABLE) ? TRUE : FALSE);
    gtk_window_set_decorated(win, (flags & SDL_NOFRAME) ? FALSE : TRUE);
    gtk_window_resize(win, width, height);
    gtk_widget_set_size_request(this->hidden->gtkdrawingarea, width, height);
    gtk_widget_show(this->hidden->gtkdrawingarea);
    gtk_widget_show(this->hidden->gtkwindow);

	/* Set up the new mode framebuffer */
	current->w = width;
	current->h = height;
    //current->format->depth = vis->bits_per_rgb;
	current->flags = flags | SDL_PREALLOC;
	current->pitch = img->bpl;
	current->pixels = this->hidden->gdkimage->mem;

	/* We're done */
	return(current);
}

static void GTKPLUS_SetCaption(_THIS, const char *title, const char *icon)
{
    gtk_window_set_title(GTK_WINDOW(this->hidden->gtkwindow),
                         (const gchar *) title);
}

/* We don't actually allow hardware surfaces. */
static int GTKPLUS_AllocHWSurface(_THIS, SDL_Surface *surface)
{
	return(-1);
}

static void GTKPLUS_FreeHWSurface(_THIS, SDL_Surface *surface)
{
}

/* We need to wait for vertical retrace on page flipped displays */
static int GTKPLUS_LockHWSurface(_THIS, SDL_Surface *surface)
{
	return(0);
}

static void GTKPLUS_UnlockHWSurface(_THIS, SDL_Surface *surface)
{
}

static void GTKPLUS_UpdateRects(_THIS, int numrects, SDL_Rect *rects)
{
    if ( (this->hidden->gtkdrawingarea != NULL) &&
         (GTK_WIDGET_DRAWABLE(this->hidden->gtkdrawingarea)) &&
         (numrects > 0) ) {
        GdkDrawable *draw = GDK_DRAWABLE(this->hidden->gtkdrawingarea->window);
        if (this->hidden->gc == NULL) {
		    this->hidden->gc = gdk_gc_new(draw);
        }

        if (this->hidden->gc != NULL) {
            GdkImage *img = this->hidden->gdkimage;
            const SDL_Rect *r = rects;
            int i;
            for (i = 0; i < numrects; i++, r++) {
                const gint x = r->x;
                const gint y = r->y;
                gdk_draw_image(draw, this->hidden->gc, img,
                               x, y, x, y, r->w, r->h);
            }
            gdk_flush(); /* transfer the GdkImage so we can make changes. */
        }
    }
}

int GTKPLUS_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors)
{
	/* !!! FIXME */
	return(0);
}

/* Note:  If we are terminated, this could be called in the middle of
   another SDL video routine -- notably UpdateRects.
*/
void GTKPLUS_VideoQuit(_THIS)
{
    int i;

    gdk_flush();

    if (this->hidden->gc != NULL) {
    	g_object_unref(this->hidden->gc);
        this->hidden->gc = NULL;
    }

	if ( this->hidden->gtkwindow ) {
        /* this deletes the drawing area widget, too. */
        gtk_widget_destroy(this->hidden->gtkwindow);
        this->hidden->gtkwindow = NULL;
    }

	if ( this->hidden->gdkimage ) {
        g_object_unref(this->hidden->gdkimage);
        this->hidden->gdkimage = NULL;
    }

    for (i = 0; i < this->hidden->nvisuals; i++) {
        g_object_unref(this->hidden->visuals[i]);
        this->hidden->visuals[i] = NULL;
    }
    this->hidden->nvisuals = 0;

	g_object_unref(this->hidden->colormap);
    this->hidden->colormap = NULL;

    gdk_flush();
}

