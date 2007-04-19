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

#include "SDL.h"
#include "../../events/SDL_sysevents.h"
#include "../../events/SDL_events_c.h"

#include "SDL_gtkvideo.h"
#include "SDL_gtkevents_c.h"

#define DEBUG_GTKPLUS_EVENTS 0
#if DEBUG_GTKPLUS_EVENTS
#define DEBUG_GTKPLUS_SIGNAL() printf("GTK+: %s\n", __FUNCTION__)
#else
#define DEBUG_GTKPLUS_SIGNAL()
#endif

static gboolean
signal_enter_notify(GtkWidget *w, GdkEventCrossing *evt, gpointer data)
{
    SET_THIS_POINTER(data);
    DEBUG_GTKPLUS_SIGNAL();
    if ( (evt->mode != GDK_CROSSING_GRAB) &&
         (evt->mode != GDK_CROSSING_UNGRAB) ) {
        if ( this->input_grab == SDL_GRAB_OFF ) {
            SDL_PrivateAppActive(1, SDL_APPMOUSEFOCUS);
        }
        SDL_PrivateMouseMotion(0, 0, evt->x, evt->y);
	}

    return FALSE;  /* don't eat event, in case app connected a handler. */
}


static gboolean
signal_leave_notify(GtkWidget *w, GdkEventCrossing *evt, gpointer data)
{
    SET_THIS_POINTER(data);
    DEBUG_GTKPLUS_SIGNAL();
    if ( (evt->mode != GDK_CROSSING_GRAB) &&
         (evt->mode != GDK_CROSSING_UNGRAB) &&
         (evt->detail != GDK_NOTIFY_INFERIOR) ) {
        if ( this->input_grab == SDL_GRAB_OFF ) {
            SDL_PrivateAppActive(0, SDL_APPMOUSEFOCUS);
        } else {
            SDL_PrivateMouseMotion(0, 0, evt->x, evt->y);
        }
    }
    return FALSE;  /* don't eat event, in case app connected a handler. */
}


static gboolean
signal_focus_in(GtkWidget *w, GdkEventFocus *evt, gpointer data)
{
    DEBUG_GTKPLUS_SIGNAL();
    SDL_PrivateAppActive(1, SDL_APPINPUTFOCUS);
    return FALSE;  /* don't eat event, in case app connected a handler. */
}


static gboolean
signal_focus_out(GtkWidget *w, GdkEventFocus *evt, gpointer data)
{
    DEBUG_GTKPLUS_SIGNAL();
    SDL_PrivateAppActive(0, SDL_APPINPUTFOCUS);
    return FALSE;  /* don't eat event, in case app connected a handler. */
}


#if 0
	    /* Generated upon EnterWindow and FocusIn */
	    case KeymapNotify: {
#ifdef DEBUG_XEVENTS
printf("KeymapNotify!\n");
#endif
		X11_SetKeyboardState(SDL_Display,  xevent.xkeymap.key_vector);
	    }
	    break;
#endif


static gboolean
signal_motion_notify(GtkWidget *w, GdkEventMotion *evt, gpointer data)
{
    SET_THIS_POINTER(data);
    DEBUG_GTKPLUS_SIGNAL();
    if ( SDL_VideoSurface ) {
        if ( 0 /* !!! FIXME mouse_relative */ ) {
            SDLGTK_WarpedMotion(this, evt);
        } else {
            SDL_PrivateMouseMotion(0, 0, evt->x, evt->y);
        }
    }

    return FALSE;  /* don't eat event, in case app connected a handler. */
}


static gboolean
signal_button_press(GtkWidget *w, GdkEventButton *evt, gpointer data)
{
    DEBUG_GTKPLUS_SIGNAL();
    SDL_PrivateMouseButton(SDL_PRESSED, evt->button, 0, 0);
    return FALSE;  /* don't eat event, in case app connected a handler. */
}

static gboolean
signal_button_release(GtkWidget *w, GdkEventButton *evt, gpointer data)
{
    DEBUG_GTKPLUS_SIGNAL();
    SDL_PrivateMouseButton(SDL_RELEASED, evt->button, 0, 0);
    return FALSE;  /* don't eat event, in case app connected a handler. */
}

static gboolean
signal_expose(GtkWidget *w, GdkEventExpose *evt, gpointer data)
{
    SET_THIS_POINTER(data);
    const GdkRectangle *area = &evt->area;
    SDL_Rect r = { area->x, area->y, area->width, area->height };
    DEBUG_GTKPLUS_SIGNAL();
    this->UpdateRects(this, 1, &r);
    return FALSE;  /* don't eat event, in case app connected a handler. */
}


static gboolean
signal_delete(GtkWidget *w, GdkEvent *evt, gpointer data)
{
    DEBUG_GTKPLUS_SIGNAL();
    SDL_PrivateQuit();
    return TRUE;  /* eat event: default handler destroys window! */
}


void GTKPLUS_ConnectSignals(_THIS)
{
    gtk_widget_set_events( this->hidden->gtkdrawingarea,
                           GDK_EXPOSURE_MASK | GDK_POINTER_MOTION_MASK |
                           GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
                           GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK |
                           GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK );

    gtk_signal_connect(GTK_OBJECT(this->hidden->gtkwindow),
                       "delete-event",
                       GTK_SIGNAL_FUNC(signal_delete), this);
    gtk_signal_connect(GTK_OBJECT(this->hidden->gtkdrawingarea),
                       "enter-notify-event",
                       GTK_SIGNAL_FUNC(signal_enter_notify), this);
    gtk_signal_connect(GTK_OBJECT(this->hidden->gtkdrawingarea),
                       "leave-notify-event",
                       GTK_SIGNAL_FUNC(signal_leave_notify), this);
    gtk_signal_connect(GTK_OBJECT(this->hidden->gtkwindow),
                       "focus-in-event",
                       GTK_SIGNAL_FUNC(signal_focus_in), this);
    gtk_signal_connect(GTK_OBJECT(this->hidden->gtkwindow),
                       "focus-out-event",
                       GTK_SIGNAL_FUNC(signal_focus_out), this);
    gtk_signal_connect(GTK_OBJECT(this->hidden->gtkdrawingarea),
                       "motion-notify-event",
                       GTK_SIGNAL_FUNC(signal_motion_notify), this);
    gtk_signal_connect(GTK_OBJECT(this->hidden->gtkdrawingarea),
                       "button-press-event",
                       GTK_SIGNAL_FUNC(signal_button_press), this);
    gtk_signal_connect(GTK_OBJECT(this->hidden->gtkdrawingarea),
                       "button-release-event",
                       GTK_SIGNAL_FUNC(signal_button_release), this);
    gtk_signal_connect(GTK_OBJECT(this->hidden->gtkdrawingarea),
                       "expose-event", GTK_SIGNAL_FUNC(signal_expose), this);
    /* !!! FIXME: more to connect here. */
}



/* !!! FIXME: more to connect here. */
#if 0
	    /* Key press? */
	    case KeyPress: {
		static SDL_keysym saved_keysym;
		SDL_keysym keysym;
		KeyCode keycode = xevent.xkey.keycode;

#ifdef DEBUG_XEVENTS
printf("KeyPress (X11 keycode = 0x%X)\n", xevent.xkey.keycode);
#endif
		/* Get the translated SDL virtual keysym */
		if ( keycode ) {
			keysym.scancode = keycode;
			keysym.sym = X11_TranslateKeycode(SDL_Display, keycode);
			keysym.mod = KMOD_NONE;
			keysym.unicode = 0;
		} else {
			keysym = saved_keysym;
		}

		/* If we're not doing translation, we're done! */
		if ( !SDL_TranslateUNICODE ) {
			posted = SDL_PrivateKeyboard(SDL_PRESSED, &keysym);
			break;
		}

		if ( XFilterEvent(&xevent, None) ) {
			if ( xevent.xkey.keycode ) {
				posted = SDL_PrivateKeyboard(SDL_PRESSED, &keysym);
			} else {
				/* Save event to be associated with IM text
				   In 1.3 we'll have a text event instead.. */
				saved_keysym = keysym;
			}
			break;
		}

		/* Look up the translated value for the key event */
#ifdef X_HAVE_UTF8_STRING
		if ( SDL_IC != NULL ) {
			static Status state;
			/* A UTF-8 character can be at most 6 bytes */
			char keybuf[6];
			if ( Xutf8LookupString(SDL_IC, &xevent.xkey,
			                        keybuf, sizeof(keybuf),
			                        NULL, &state) ) {
				keysym.unicode = Utf8ToUcs4((Uint8*)keybuf);
			}
		}
		else
#endif
		{
			static XComposeStatus state;
			char keybuf[32];

			if ( XLookupString(&xevent.xkey,
			                    keybuf, sizeof(keybuf),
			                    NULL, &state) ) {
				/*
				* FIXME: XLookupString() may yield more than one
				* character, so we need a mechanism to allow for
				* this (perhaps null keypress events with a
				* unicode value)
				*/
				keysym.unicode = (Uint8)keybuf[0];
			}
		}
		posted = SDL_PrivateKeyboard(SDL_PRESSED, &keysym);
	    }
	    break;

	    /* Key release? */
	    case KeyRelease: {
		SDL_keysym keysym;
		KeyCode keycode = xevent.xkey.keycode;

#ifdef DEBUG_XEVENTS
printf("KeyRelease (X11 keycode = 0x%X)\n", xevent.xkey.keycode);
#endif
		/* Check to see if this is a repeated key */
		if ( X11_KeyRepeat(SDL_Display, &xevent) ) {
			break;
		}

		/* Get the translated SDL virtual keysym */
		keysym.scancode = keycode;
		keysym.sym = X11_TranslateKeycode(SDL_Display, keycode);
		keysym.mod = KMOD_NONE;
		keysym.unicode = 0;

		posted = SDL_PrivateKeyboard(SDL_RELEASED, &keysym);
	    }
	    break;

	    /* Have we been iconified? */
	    case UnmapNotify: {
#ifdef DEBUG_XEVENTS
printf("UnmapNotify!\n");
#endif
		/* If we're active, make ourselves inactive */
		if ( SDL_GetAppState() & SDL_APPACTIVE ) {
			/* Swap out the gamma before we go inactive */
			X11_SwapVidModeGamma(this);

			/* Send an internal deactivate event */
			posted = SDL_PrivateAppActive(0,
					SDL_APPACTIVE|SDL_APPINPUTFOCUS);
		}
	    }
	    break;

	    /* Have we been restored? */
	    case MapNotify: {
#ifdef DEBUG_XEVENTS
printf("MapNotify!\n");
#endif
		/* If we're not active, make ourselves active */
		if ( !(SDL_GetAppState() & SDL_APPACTIVE) ) {
			/* Send an internal activate event */
			posted = SDL_PrivateAppActive(1, SDL_APPACTIVE);

			/* Now that we're active, swap the gamma back */
			X11_SwapVidModeGamma(this);
		}

		if ( SDL_VideoSurface &&
		     (SDL_VideoSurface->flags & SDL_FULLSCREEN) ) {
			X11_EnterFullScreen(this);
		} else {
			X11_GrabInputNoLock(this, this->input_grab);
		}
		X11_CheckMouseModeNoLock(this);

		if ( SDL_VideoSurface ) {
			X11_RefreshDisplay(this);
		}
	    }
	    break;

	    /* Have we been resized or moved? */
	    case ConfigureNotify: {
#ifdef DEBUG_XEVENTS
printf("ConfigureNotify! (resize: %dx%d)\n", xevent.xconfigure.width, xevent.xconfigure.height);
#endif
		if ( SDL_VideoSurface ) {
		    if ((xevent.xconfigure.width != SDL_VideoSurface->w) ||
		        (xevent.xconfigure.height != SDL_VideoSurface->h)) {
			/* FIXME: Find a better fix for the bug with KDE 1.2 */
			if ( ! ((xevent.xconfigure.width == 32) &&
			        (xevent.xconfigure.height == 32)) ) {
				SDL_PrivateResize(xevent.xconfigure.width,
				                  xevent.xconfigure.height);
			}
		    } else {
			/* OpenGL windows need to know about the change */
			if ( SDL_VideoSurface->flags & SDL_OPENGL ) {
				SDL_PrivateExpose();
			}
		    }
		}
	    }
	    break;

	    /* Have we been requested to quit (or another client message?) */
	    case ClientMessage: {
		if ( (xevent.xclient.format == 32) &&
		     (xevent.xclient.data.l[0] == WM_DELETE_WINDOW) )
		{
			posted = SDL_PrivateQuit();
		} else
		if ( SDL_ProcessEvents[SDL_SYSWMEVENT] == SDL_ENABLE ) {
			SDL_SysWMmsg wmmsg;

			SDL_VERSION(&wmmsg.version);
			wmmsg.subsystem = SDL_SYSWM_X11;
			wmmsg.event.xevent = xevent;
			posted = SDL_PrivateSysWMEvent(&wmmsg);
		}
	    }
	    break;



	    default: {
#ifdef DEBUG_XEVENTS
printf("Unhandled event %d\n", xevent.type);
#endif
		/* Only post the event if we're watching for it */
		if ( SDL_ProcessEvents[SDL_SYSWMEVENT] == SDL_ENABLE ) {
			SDL_SysWMmsg wmmsg;

			SDL_VERSION(&wmmsg.version);
			wmmsg.subsystem = SDL_SYSWM_X11;
			wmmsg.event.xevent = xevent;
			posted = SDL_PrivateSysWMEvent(&wmmsg);
		}
	    }
	    break;
	}
#endif



void GTKPLUS_PumpEvents(_THIS)
{
    while (gtk_events_pending())
	    gtk_main_iteration();
}

void GTKPLUS_InitOSKeymap(_THIS)
{
    /* !!! FIXME */
	/* do nothing. */
}

/* end of SDL_gtkevents.c ... */

