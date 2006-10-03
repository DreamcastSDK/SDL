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

/* Allow access to a raw mixing buffer */

#include "SDL.h"
#include "SDL_audio_c.h"
#include "SDL_audiomem.h"
#include "SDL_sysaudio.h"

#ifdef __OS2__
/* We'll need the DosSetPriority() API! */
#define INCL_DOSPROCESS
#include <os2.h>
#endif

static SDL_AudioDriver current_audio;

/* !!! FIXME: don't use a static array, but it's Good Enough For Now... */
static SDL_AudioDevice *open_devices[16];


/* Available audio drivers */
static AudioBootStrap *bootstrap[] = {
#if SDL_AUDIO_DRIVER_BSD
    &BSD_AUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_OSS
    &DSP_bootstrap,
    &DMA_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_ALSA
    &ALSA_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_QNXNTO
    &QNXNTOAUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_SUNAUDIO
    &SUNAUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_DMEDIA
    &DMEDIA_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_ARTS
    &ARTS_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_ESD
    &ESD_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_NAS
    &NAS_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_DSOUND
    &DSOUND_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_WAVEOUT
    &WAVEOUT_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_PAUD
    &Paud_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_BAUDIO
    &BAUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_COREAUDIO
    &COREAUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_SNDMGR
    &SNDMGR_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_AHI
    &AHI_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_MINT
    &MINTAUDIO_GSXB_bootstrap,
    &MINTAUDIO_MCSN_bootstrap,
    &MINTAUDIO_STFA_bootstrap,
    &MINTAUDIO_XBIOS_bootstrap,
    &MINTAUDIO_DMA8_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_DISK
    &DISKAUD_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_DUMMY
    &DUMMYAUD_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_DC
    &DCAUD_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_MMEAUDIO
    &MMEAUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_DART
    &DART_bootstrap,
#endif
    NULL
};

#if SDL_AUDIO_DRIVER_AHI
static int audio_configured = 0;
#endif

static SDL_AudioDevice *get_audio_device(SDL_AudioDeviceID id)
{
    id--;
    if ( (id >= SDL_arraysize(open_devices)) || (open_devices[id] == NULL) ) {
        SDL_SetError("Invalid audio device ID");
        return NULL;
    }

    return open_devices[id];
}


/* The general mixing thread function */
int SDLCALL
SDL_RunAudio(void *devicep)
{
    SDL_AudioDevice *device = (SDL_AudioDevice *) devicep;
    const int legacy_device = (device == open_devices[0]);
    Uint8 *stream;
    int stream_len;
    void *udata;
    void (SDLCALL * fill) (void *userdata, Uint8 * stream, int len);
    int silence;

/* !!! FIXME: can we push this into the Amiga driver? */
#if SDL_AUDIO_DRIVER_AHI
#error this is probably broken.
    int started = 0;

/* AmigaOS NEEDS that the audio driver is opened in the thread that uses it! */

    D(bug("Task audio started audio struct:<%lx>...\n", audiop));

    D(bug("Before Openaudio..."));
    if (audio->OpenAudio(audio, &audio->spec) == -1) {
        D(bug("Open audio failed...\n"));
        return (-1);
    }
    D(bug("OpenAudio...OK\n"));
#endif

    /* Perform any thread setup */
    if (current_audio.impl.ThreadInit != NULL) {
        current_audio.impl.ThreadInit(device);
    }
    device->threadid = SDL_ThreadID();

    /* Set up the mixing function */
    fill = device->spec.callback;
    udata = device->spec.userdata;

#if SDL_AUDIO_DRIVER_AHI
    audio_configured = 1;

    D(bug("Audio configured... Checking for conversion\n"));
    SDL_mutexP(audio->mixer_lock);
    D(bug("Semaphore obtained...\n"));
#endif

    if (device->convert.needed) {
        if (device->convert.src_format == AUDIO_U8) {
            silence = 0x80;
        } else {
            silence = 0;
        }
        stream_len = device->convert.len;
    } else {
        silence = device->spec.silence;
        stream_len = device->spec.size;
    }

#if SDL_AUDIO_DRIVER_AHI
    SDL_mutexV(device->mixer_lock);
    D(bug("Entering audio loop...\n"));
#endif

/* !!! FIXME: push this out of core. */
#ifdef __OS2__
    /* Increase the priority of this thread to make sure that
       the audio will be continuous all the time! */
#ifdef USE_DOSSETPRIORITY
    if (SDL_getenv("SDL_USE_TIMECRITICAL_AUDIO")) {
#ifdef DEBUG_BUILD
        printf
            ("[SDL_RunAudio] : Setting priority to TimeCritical+0! (TID%d)\n",
             SDL_ThreadID());
#endif
        DosSetPriority(PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, 0);
    } else {
#ifdef DEBUG_BUILD
        printf
            ("[SDL_RunAudio] : Setting priority to ForegroundServer+0! (TID%d)\n",
             SDL_ThreadID());
#endif
        DosSetPriority(PRTYS_THREAD, PRTYC_FOREGROUNDSERVER, 0, 0);
    }
#endif
#endif

    /* Loop, filling the audio buffers */
    while (device->enabled) {

        /* Fill the current buffer with sound */
        if (device->convert.needed) {
            if (device->convert.buf) {
                stream = device->convert.buf;
            } else {
                continue;
            }
        } else {
            stream = current_audio.impl.GetDeviceBuf(device);
            if (stream == NULL) {
                stream = device->fake_stream;
            }
        }

        /* New code should fill buffer or set it to silence themselves. */
        if (legacy_device) {
            SDL_memset(stream, silence, stream_len);
        }

        if (!device->paused) {
            SDL_mutexP(device->mixer_lock);
            (*fill) (udata, stream, stream_len);
            SDL_mutexV(device->mixer_lock);
        }

        /* Convert the audio if necessary */
        if (device->convert.needed) {
            SDL_ConvertAudio(&device->convert);
            stream = current_audio.impl.GetDeviceBuf(device);
            if (stream == NULL) {
                stream = device->fake_stream;
            }
            SDL_memcpy(stream, device->convert.buf, device->convert.len_cvt);
        }

        /* Ready current buffer for play and change current buffer */
        if (stream != device->fake_stream) {
            current_audio.impl.PlayDevice(device);
        }

        /* Wait for an audio buffer to become available */
        if (stream == device->fake_stream) {
            SDL_Delay((device->spec.samples * 1000) / device->spec.freq);
        } else {
            current_audio.impl.WaitDevice(device);
        }
    }

    /* Wait for the audio to drain.. */
    if (current_audio.impl.WaitDone) {
        current_audio.impl.WaitDone(device);
    }
#if SDL_AUDIO_DRIVER_AHI
    D(bug("WaitDevice...Done\n"));

    audio->CloseDevice(audio);

    D(bug("CloseDevice..Done, subtask exiting...\n"));
    audio_configured = 0;
#endif
#ifdef __OS2__
#ifdef DEBUG_BUILD
    printf("[SDL_RunAudio] : Task exiting. (TID%d)\n", SDL_ThreadID());
#endif
#endif
    return (0);
}

static void
SDL_LockDevice_Default(SDL_AudioDevice * audio)
{
    if (audio->thread && (SDL_ThreadID() == audio->threadid)) {
        return;
    }
    SDL_mutexP(audio->mixer_lock);
}

static void
SDL_UnlockDevice_Default(SDL_AudioDevice * audio)
{
    if (audio->thread && (SDL_ThreadID() == audio->threadid)) {
        return;
    }
    SDL_mutexV(audio->mixer_lock);
}

static SDL_AudioFormat
SDL_ParseAudioFormat(const char *string)
{
    #define CHECK_FMT_STRING(x) if (strcmp(string, #x) == 0) return AUDIO_##x
    CHECK_FMT_STRING(U8);
    CHECK_FMT_STRING(S8);
    CHECK_FMT_STRING(U16LSB);
    CHECK_FMT_STRING(S16LSB);
    CHECK_FMT_STRING(U16MSB);
    CHECK_FMT_STRING(S16MSB);
    CHECK_FMT_STRING(U16SYS);
    CHECK_FMT_STRING(S16SYS);
    CHECK_FMT_STRING(U16);
    CHECK_FMT_STRING(S16);
    CHECK_FMT_STRING(S32LSB);
    CHECK_FMT_STRING(S32MSB);
    CHECK_FMT_STRING(S32SYS);
    CHECK_FMT_STRING(S32);
    CHECK_FMT_STRING(F32LSB);
    CHECK_FMT_STRING(F32MSB);
    CHECK_FMT_STRING(F32SYS);
    CHECK_FMT_STRING(F32);
    #undef CHECK_FMT_STRING
    return 0;
}

int
SDL_GetNumAudioDrivers(void)
{
    return (SDL_arraysize(bootstrap) - 1);
}

const char *
SDL_GetAudioDriver(int index)
{
    if (index >= 0 && index < SDL_GetNumAudioDrivers()) {
        return (bootstrap[index]->name);
    }
    return (NULL);
}

int
SDL_AudioInit(const char *driver_name)
{
    int i = 0;
    int initialized = 0;

    if (SDL_WasInit(SDL_INIT_AUDIO)) {
        SDL_AudioQuit();  /* shutdown driver if already running. */
    }

    SDL_memset(&current_audio, '\0', sizeof (current_audio));
    SDL_memset(open_devices, '\0', sizeof (open_devices));

    /* !!! FIXME: build a priority list of available drivers... */

    /* Select the proper audio driver */
    if (driver_name == NULL) {
        driver_name = SDL_getenv("SDL_AUDIODRIVER");
    }

/* !!! FIXME: move this esound shite into the esound driver... */
#if SDL_AUDIO_DRIVER_ESD
    if ((driver_name == NULL) && (SDL_getenv("ESPEAKER") != NULL)) {
        /* Ahem, we know that if ESPEAKER is set, user probably wants
           to use ESD, but don't start it if it's not already running.
           This probably isn't the place to do this, but... Shh! :)
         */
        for (i = 0; bootstrap[i]; ++i) {
            if (SDL_strcasecmp(bootstrap[i]->name, "esd") == 0) {
#ifdef HAVE_PUTENV
                const char *esd_no_spawn;

                /* Don't start ESD if it's not running */
                esd_no_spawn = getenv("ESD_NO_SPAWN");
                if (esd_no_spawn == NULL) {
                    putenv("ESD_NO_SPAWN=1");
                }
#endif
                if (bootstrap[i]->available()) {
                    SDL_memset(&current_audio, 0, sizeof (current_audio));
                    current_audio.name = bootstrap[i]->name;
                    current_audio.desc = bootstrap[i]->desc;
                    initialized = bootstrap[i]->init(&current_audio.impl);
                    break;
                }
#ifdef HAVE_UNSETENV
                if (esd_no_spawn == NULL) {
                    unsetenv("ESD_NO_SPAWN");
                }
#endif
            }
        }
    }
#endif /* SDL_AUDIO_DRIVER_ESD */

    if (!initialized) {
        if (driver_name != NULL) {
            for (i = 0; bootstrap[i]; ++i) {
                if (SDL_strcasecmp(bootstrap[i]->name, driver_name) == 0) {
                    if (bootstrap[i]->available()) {
                        SDL_memset(&current_audio, 0, sizeof (current_audio));
                        current_audio.name = bootstrap[i]->name;
                        current_audio.desc = bootstrap[i]->desc;
                        initialized = bootstrap[i]->init(&current_audio.impl);
                    }
                    break;
                }
            }
        } else {
            for (i = 0; (!initialized) && (bootstrap[i]); ++i) {
                if (bootstrap[i]->available()) {
                    SDL_memset(&current_audio, 0, sizeof (current_audio));
                    current_audio.name = bootstrap[i]->name;
                    current_audio.desc = bootstrap[i]->desc;
                    initialized = bootstrap[i]->init(&current_audio.impl);
                }
            }
        }
        if (!initialized) {
            if (driver_name) {
                SDL_SetError("%s not available", driver_name);
            } else {
                SDL_SetError("No available audio device");
            }
            SDL_memset(&current_audio, 0, sizeof (current_audio));
            return (-1);  /* No driver was available, so fail. */
        }
    }

    if (!current_audio.impl.LockDevice && !current_audio.impl.UnlockDevice) {
        current_audio.impl.LockDevice = SDL_LockDevice_Default;
        current_audio.impl.UnlockDevice = SDL_UnlockDevice_Default;
    }

    return (0);
}

/*
 * Get the current audio driver name
 */
const char *
SDL_GetCurrentAudioDriver()
{
    return current_audio.name;
}


int
SDL_GetNumAudioDevices(int iscapture)
{
    if (!SDL_WasInit(SDL_INIT_AUDIO) || !current_audio.impl.DetectDevices) {
        return -1;
    }
    return current_audio.impl.DetectDevices(iscapture);
}


const char *
SDL_GetAudioDeviceName(int index, int iscapture)
{
    if (!SDL_WasInit(SDL_INIT_AUDIO)) {
        SDL_SetError("Audio subsystem is not initialized");
        return NULL;
    }

    if ((index < 0) && (!current_audio.impl.GetDeviceName)) {
        SDL_SetError("No such device");
        return NULL;
    }

    return current_audio.impl.GetDeviceName(index, iscapture);
}


static void
close_audio_device(SDL_AudioDevice *device)
{
    device->enabled = 0;
    if (device->thread != NULL) {
        SDL_WaitThread(device->thread, NULL);
    }
    if (device->mixer_lock != NULL) {
        SDL_DestroyMutex(device->mixer_lock);
    }
    if (device->fake_stream != NULL) {
        SDL_FreeAudioMem(device->fake_stream);
    }
    if (device->convert.needed) {
        SDL_FreeAudioMem(device->convert.buf);
    }
#if !SDL_AUDIO_DRIVER_AHI   /* !!! FIXME: get rid of this #if. */
    if (device->opened) {
        current_audio.impl.CloseDevice(device);
        device->opened = 0;
    }
#endif
    SDL_FreeAudioMem(device);
}


/*
 * Sanity check desired AudioSpec for SDL_OpenAudio() in (orig).
 *  Fills in a sanitized copy in (prepared).
 *  Returns non-zero if okay, zero on fatal parameters in (orig).
 */
static int
prepare_audiospec(const SDL_AudioSpec *orig, SDL_AudioSpec *prepared)
{
    SDL_memcpy(prepared, orig, sizeof (SDL_AudioSpec));

    if (orig->callback == NULL) {
        SDL_SetError("SDL_OpenAudio() passed a NULL callback");
        return 0;
    }

    if (orig->freq == 0) {
        const char *env = SDL_getenv("SDL_AUDIO_FREQUENCY");
        if ( (!env) || ((prepared->freq = SDL_atoi(env)) == 0) ) {
            prepared->freq = 22050;  /* a reasonable default */
        }
    }

    if (orig->format == 0) {
        const char *env = SDL_getenv("SDL_AUDIO_FORMAT");
        if ((!env) || ((prepared->format = SDL_ParseAudioFormat(env)) == 0)) {
            prepared->format = AUDIO_S16;  /* a reasonable default */
        }
    }

    switch (orig->channels) {
    case 0: {
        const char *env = SDL_getenv("SDL_AUDIO_CHANNELS");
        if ( (!env) || ((prepared->channels = SDL_atoi(env)) == 0) ) {
            prepared->channels = 2;  /* a reasonable default */
        }
        break;
    }
    case 1:                    /* Mono */
    case 2:                    /* Stereo */
    case 4:                    /* surround */
    case 6:                    /* surround with center and lfe */
        break;
    default:
        SDL_SetError("Unsupported number of audio channels.");
        return 0;
    }

    if (orig->samples == 0) {
        const char *env = SDL_getenv("SDL_AUDIO_SAMPLES");
        if ( (!env) || ((prepared->samples = (Uint16) SDL_atoi(env)) == 0) ) {
            /* Pick a default of ~46 ms at desired frequency */
            /* !!! FIXME: remove this when the non-Po2 resampling is in. */
            const int samples = (prepared->freq / 1000) * 46;
            int power2 = 1;
            while (power2 < samples) {
                power2 *= 2;
            }
            prepared->samples = power2;
        }
    }

    /* Calculate the silence and size of the audio specification */
    SDL_CalculateAudioSpec(prepared);

    return 1;
}


static SDL_AudioDeviceID
open_audio_device(const char *devname, int iscapture,
                    const SDL_AudioSpec *_desired, SDL_AudioSpec *obtained,
                    int min_id)
{
    int i = 0;
    SDL_AudioDeviceID id = 0;
    SDL_AudioSpec desired;
    SDL_AudioDevice *device;

    if (iscapture) {
        SDL_SetError("Audio capture support not implemented yet!");
        return 0;  /* !!! FIXME */
    }

    if (!SDL_WasInit(SDL_INIT_AUDIO)) {
        SDL_SetError("Audio subsystem is not initialized");
        return 0;
    }

    if (!prepare_audiospec(_desired, &desired)) {
        return 0;
    }

    /* If app doesn't care about a specific device, let the user override. */
    if (devname == NULL) {
        devname = SDL_getenv("SDL_AUDIO_DEVICE_NAME");
    }

    device = (SDL_AudioDevice *) SDL_AllocAudioMem(sizeof (SDL_AudioDevice));
    if (device == NULL) {
        SDL_OutOfMemory();
        return 0;
    }
    SDL_memset(device, '\0', sizeof (SDL_AudioDevice));
    SDL_memcpy(&device->spec, &desired, sizeof (SDL_AudioSpec));
    device->driver = &current_audio;  /* !!! FIXME: unused... */
    device->enabled = 1;
    device->paused = 1;

/* !!! FIXME: Get this out of the core. */
#if defined(__MINT__) && SDL_THREADS_DISABLED
    /* Uses interrupt driven audio, without thread */
#else
    /* Create a semaphore for locking the sound buffers */
    device->mixer_lock = SDL_CreateMutex();
    if (device->mixer_lock == NULL) {
        SDL_SetError("Couldn't create mixer lock");
        return 0;
    }
#endif /* __MINT__ */

/* !!! FIXME: Get this #if out of the core. */
/* AmigaOS opens audio inside the main loop */
#if !SDL_AUDIO_DRIVER_AHI
    if (!current_audio.impl.OpenDevice(device, devname, iscapture)) {
        close_audio_device(device);
        return 0;
    }
    device->opened = 2;  /* !!! FIXME */
#else
#   error needs to be fixed for new internal API. Email Ryan for details.

    D(bug("Locking semaphore..."));
    SDL_mutexP(audio->mixer_lock);


    audio->thread = SDL_CreateThread(SDL_RunAudio, audio);
    D(bug("Created thread...\n"));

    if (audio->thread == NULL) {
        SDL_mutexV(audio->mixer_lock);
        SDL_CloseAudio();
        SDL_SetError("Couldn't create audio thread");
        return 0;
    }

    while (!audio_configured)
        SDL_Delay(100);
#endif

    /* If the audio driver changes the buffer size, accept it */
    if (device->spec.samples != desired.samples) {
        desired.samples = device->spec.samples;
        SDL_CalculateAudioSpec(&device->spec);
    }

    /* Allocate a fake audio memory buffer */
    device->fake_stream = SDL_AllocAudioMem(device->spec.size);
    if (device->fake_stream == NULL) {
        close_audio_device(device);
        SDL_OutOfMemory();
        return 0;
    }

    /* See if we need to do any conversion */
    if (obtained != NULL) {
        SDL_memcpy(obtained, &device->spec, sizeof(SDL_AudioSpec));
    } else if (desired.freq != device->spec.freq ||
               desired.format != device->spec.format ||
               desired.channels != device->spec.channels) {
        /* Build an audio conversion block */
        if (SDL_BuildAudioCVT(&device->convert,
                              desired.format, desired.channels,
                              desired.freq,
                              device->spec.format, device->spec.channels,
                              device->spec.freq) < 0) {
            close_audio_device(device);
            return 0;
        }
        if (device->convert.needed) {
            device->convert.len = desired.size;
            device->convert.buf =
                (Uint8 *) SDL_AllocAudioMem(device->convert.len *
                                            device->convert.len_mult);
            if (device->convert.buf == NULL) {
                close_audio_device(device);
                SDL_OutOfMemory();
                return 0;
            }
        }
    }

    /* Find an available device ID and store the structure... */
    for (id = min_id-1; id < SDL_arraysize(open_devices); id++) {
        if (open_devices[id] == NULL) {
            open_devices[id] = device;
            break;
        }
    }

    /* !!! FIXME: remove static array... */
    if (id == SDL_arraysize(open_devices)) {
        SDL_SetError("Too many open audio devices");
        close_audio_device(device);
        return 0;
    }

#if !SDL_AUDIO_DRIVER_AHI
    /* Start the audio thread if necessary */
    switch (device->opened) {  /* !!! FIXME: what is this?! */
    case 1:
        /* Start the audio thread */
/* !!! FIXME: this is nasty. */
#if (defined(__WIN32__) && !defined(_WIN32_WCE)) && !defined(HAVE_LIBC)
#undef SDL_CreateThread
        device->thread = SDL_CreateThread(SDL_RunAudio, device, NULL, NULL);
#else
        device->thread = SDL_CreateThread(SDL_RunAudio, device);
#endif
        if (device->thread == NULL) {
            SDL_CloseAudioDevice(id+1);
            SDL_SetError("Couldn't create audio thread");
            return 0;
        }
        break;

    default:
        /* The audio is now playing */
        break;
    }
#else
    SDL_mutexV(audio->mixer_lock);
    D(bug("SDL_OpenAudio USCITA...\n"));

#endif

    return id+1;
}


int
SDL_OpenAudio(const SDL_AudioSpec * desired, SDL_AudioSpec * obtained)
{
    SDL_AudioDeviceID id = 0;

    /* Start up the audio driver, if necessary. This is legacy behaviour! */
    if (!SDL_WasInit(SDL_INIT_AUDIO)) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
            return (-1);
        }
    }

    /* SDL_OpenAudio() is legacy and can only act on Device ID #1. */
    if (open_devices[0] != NULL) {
        SDL_SetError("Audio device is already opened");
        return (-1);
    }

    id = open_audio_device(NULL, 0, desired, obtained, 1);
    if (id > 1) {  /* this should never happen in theory... */
        SDL_CloseAudioDevice(id);
        SDL_SetError("Internal error");  /* MUST be Device ID #1! */
        return (-1);
    }

    return ((id == 0) ? -1 : 0);
}

SDL_AudioDeviceID
SDL_OpenAudioDevice(const char *device, int iscapture,
                    const SDL_AudioSpec *desired, SDL_AudioSpec *obtained)
{
    return open_audio_device(device, iscapture, desired, obtained, 2);
}

SDL_audiostatus
SDL_GetAudioDeviceStatus(SDL_AudioDeviceID devid)
{
    SDL_AudioDevice *device = get_audio_device(devid);
    SDL_audiostatus status = SDL_AUDIO_STOPPED;
    if (device && device->enabled) {
        if (device->paused) {
            status = SDL_AUDIO_PAUSED;
        } else {
            status = SDL_AUDIO_PLAYING;
        }
    }
    return (status);
}


SDL_audiostatus
SDL_GetAudioStatus(void)
{
    return SDL_GetAudioDeviceStatus(1);
}

void
SDL_PauseAudioDevice(SDL_AudioDeviceID devid, int pause_on)
{
    SDL_AudioDevice *device = get_audio_device(devid);
    if (device) {
        device->paused = pause_on;
    }
}

void
SDL_PauseAudio(int pause_on)
{
    SDL_PauseAudioDevice(1, pause_on);
}


void
SDL_LockAudioDevice(SDL_AudioDeviceID devid)
{
    if (current_audio.impl.LockDevice != NULL) {
        SDL_AudioDevice *device = get_audio_device(devid);
        /* Obtain a lock on the mixing buffers */
        if (device) {
            current_audio.impl.LockDevice(device);
        }
    }
}

void
SDL_LockAudio(void)
{
    SDL_LockAudioDevice(1);
}

void
SDL_UnlockAudioDevice(SDL_AudioDeviceID devid)
{
    if (current_audio.impl.UnlockDevice != NULL) {
        SDL_AudioDevice *device = get_audio_device(devid);
        /* Obtain a lock on the mixing buffers */
        if (device) {
            current_audio.impl.UnlockDevice(device);
        }
    }
}

void
SDL_UnlockAudio(void)
{
    SDL_UnlockAudioDevice(1);
}

void
SDL_CloseAudioDevice(SDL_AudioDeviceID devid)
{
    SDL_AudioDevice *device = get_audio_device(devid);
    if (device) {
        close_audio_device(device);
        open_devices[devid-1] = NULL;
    }
}

void
SDL_CloseAudio(void)
{
    SDL_CloseAudioDevice(1);
}

void
SDL_AudioQuit(void)
{
    SDL_AudioDeviceID i;
    for (i = 0; i < SDL_arraysize(open_devices); i++) {
        SDL_CloseAudioDevice(i);
    }

    /* Free the driver data */
    current_audio.impl.Deinitialize();
    SDL_memset(&current_audio, '\0', sizeof (current_audio));
    SDL_memset(open_devices, '\0', sizeof (open_devices));
}

#define NUM_FORMATS 10
static int format_idx;
static int format_idx_sub;
static SDL_AudioFormat format_list[NUM_FORMATS][NUM_FORMATS] = {
    {AUDIO_U8, AUDIO_S8, AUDIO_S16LSB, AUDIO_S16MSB, AUDIO_U16LSB,
     AUDIO_U16MSB, AUDIO_S32LSB, AUDIO_S32MSB, AUDIO_F32LSB, AUDIO_F32MSB},
    {AUDIO_S8, AUDIO_U8, AUDIO_S16LSB, AUDIO_S16MSB, AUDIO_U16LSB,
     AUDIO_U16MSB, AUDIO_S32LSB, AUDIO_S32MSB, AUDIO_F32LSB, AUDIO_F32MSB},
    {AUDIO_S16LSB, AUDIO_S16MSB, AUDIO_U16LSB, AUDIO_U16MSB, AUDIO_S32LSB,
     AUDIO_S32MSB, AUDIO_F32LSB, AUDIO_F32MSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_S16MSB, AUDIO_S16LSB, AUDIO_U16MSB, AUDIO_U16LSB, AUDIO_S32MSB,
     AUDIO_S32LSB, AUDIO_F32MSB, AUDIO_F32LSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_U16LSB, AUDIO_U16MSB, AUDIO_S16LSB, AUDIO_S16MSB, AUDIO_S32LSB,
     AUDIO_S32MSB, AUDIO_F32LSB, AUDIO_F32MSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_U16MSB, AUDIO_U16LSB, AUDIO_S16MSB, AUDIO_S16LSB, AUDIO_S32MSB,
     AUDIO_S32LSB, AUDIO_F32MSB, AUDIO_F32LSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_S32LSB, AUDIO_S32MSB, AUDIO_F32LSB, AUDIO_F32MSB, AUDIO_S16LSB,
     AUDIO_S16MSB, AUDIO_U16LSB, AUDIO_U16MSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_S32MSB, AUDIO_S32LSB, AUDIO_F32MSB, AUDIO_F32LSB, AUDIO_S16MSB,
     AUDIO_S16LSB, AUDIO_U16MSB, AUDIO_U16LSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_F32LSB, AUDIO_F32MSB, AUDIO_S32LSB, AUDIO_S32MSB, AUDIO_S16LSB,
     AUDIO_S16MSB, AUDIO_U16LSB, AUDIO_U16MSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_F32MSB, AUDIO_F32LSB, AUDIO_S32MSB, AUDIO_S32LSB, AUDIO_S16MSB,
     AUDIO_S16LSB, AUDIO_U16MSB, AUDIO_U16LSB, AUDIO_U8, AUDIO_S8},
};

SDL_AudioFormat
SDL_FirstAudioFormat(SDL_AudioFormat format)
{
    for (format_idx = 0; format_idx < NUM_FORMATS; ++format_idx) {
        if (format_list[format_idx][0] == format) {
            break;
        }
    }
    format_idx_sub = 0;
    return (SDL_NextAudioFormat());
}

SDL_AudioFormat
SDL_NextAudioFormat(void)
{
    if ((format_idx == NUM_FORMATS) || (format_idx_sub == NUM_FORMATS)) {
        return (0);
    }
    return (format_list[format_idx][format_idx_sub++]);
}

void
SDL_CalculateAudioSpec(SDL_AudioSpec * spec)
{
    switch (spec->format) {
    case AUDIO_U8:
        spec->silence = 0x80;
        break;
    default:
        spec->silence = 0x00;
        break;
    }
    spec->size = SDL_AUDIO_BITSIZE(spec->format) / 8;
    spec->size *= spec->channels;
    spec->size *= spec->samples;
}


/*
 * Moved here from SDL_mixer.c, since it relies on internals of an opened
 *  audio device (and is deprecated, by the way!).
 */
void
SDL_MixAudio(Uint8 * dst, const Uint8 * src, Uint32 len, int volume)
{
    /* Mix the user-level audio format */
    SDL_AudioDevice *device = get_audio_device(1);
    if (device != NULL) {
        SDL_AudioFormat format;
        if (device->convert.needed) {
            format = device->convert.src_format;
        } else {
            format = device->spec.format;
        }
        SDL_MixAudioFormat(dst, src, format, len, volume);
    }
}

/* vi: set ts=4 sw=4 expandtab: */
