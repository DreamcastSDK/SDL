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

#include <AudioUnit/AudioUnit.h>

#include "SDL_audio.h"
#include "../SDL_audio_c.h"
#include "../SDL_sysaudio.h"
#include "SDL_coreaudio.h"


/* Audio driver functions */

static int COREAUDIO_OpenAudio(_THIS, const char *devname, int iscapture);
static void COREAUDIO_WaitAudio(_THIS);
static void COREAUDIO_PlayAudio(_THIS);
static Uint8 *COREAUDIO_GetAudioBuf(_THIS);
static void COREAUDIO_CloseAudio(_THIS);

/* Audio driver bootstrap functions */

static int
COREAUDIO_Available(void)
{
    return (1);
}

static int
COREAUDIO_Init(SDL_AudioDriverImpl *impl)
{
    /* Set the function pointers */
    impl->OpenAudio = COREAUDIO_OpenAudio;
    impl->WaitAudio = COREAUDIO_WaitAudio;
    impl->PlayAudio = COREAUDIO_PlayAudio;
    impl->GetAudioBuf = COREAUDIO_GetAudioBuf;
    impl->CloseAudio = COREAUDIO_CloseAudio;

    return 1;
}

AudioBootStrap COREAUDIO_bootstrap = {
    "coreaudio", "Mac OS X CoreAudio",
    COREAUDIO_Available, COREAUDIO_Init
};

/* The CoreAudio callback */
static OSStatus
audioCallback(void *inRefCon,
              AudioUnitRenderActionFlags inActionFlags,
              const AudioTimeStamp * inTimeStamp,
              UInt32 inBusNumber, AudioBuffer * ioData)
{
    SDL_AudioDevice *this = (SDL_AudioDevice *) inRefCon;
    UInt32 remaining, len;
    void *ptr;

    /* Only do anything if audio is enabled and not paused */
    if (!this->enabled || this->paused) {
        SDL_memset(ioData->mData, this->spec.silence, ioData->mDataByteSize);
        return 0;
    }

    /* No SDL conversion should be needed here, ever, since we accept
       any input format in OpenAudio, and leave the conversion to CoreAudio.
     */
    /*
       assert(!this->convert.needed);
       assert(this->spec.channels == ioData->mNumberChannels);
     */

    remaining = ioData->mDataByteSize;
    ptr = ioData->mData;
    while (remaining > 0) {
        if (this->hidden->bufferOffset >= this->hidden->bufferSize) {
            /* Generate the data */
            SDL_memset(this->hidden->buffer, this->spec.silence,
                       this->hidden->bufferSize);
            SDL_mutexP(this->mixer_lock);
            (*this->spec.callback) (this->spec.userdata, this->hidden->buffer,
                                    this->hidden->bufferSize);
            SDL_mutexV(this->mixer_lock);
            this->hidden->bufferOffset = 0;
        }

        len = this->hidden->bufferSize - this->hidden->bufferOffset;
        if (len > remaining)
            len = remaining;
        SDL_memcpy(ptr,
                    (char *) this->hidden->buffer + this->hidden->bufferOffset,
                    len);
        ptr = (char *) ptr + len;
        remaining -= len;
        this->hidden->bufferOffset += len;
    }

    return 0;
}

/* Dummy functions -- we don't use thread-based audio */
void
COREAUDIO_WaitAudio(_THIS)
{
    return;
}

void
COREAUDIO_PlayAudio(_THIS)
{
    return;
}

Uint8 *
COREAUDIO_GetAudioBuf(_THIS)
{
    return (NULL);
}

void
COREAUDIO_CloseAudio(_THIS)
{
    OSStatus result;
    struct AudioUnitInputCallback callback;

    if (this->hidden == NULL) {
        return;
    }

    /* stop processing the audio unit */
    result = AudioOutputUnitStop(this->hidden->outputAudioUnit);
    if (result != noErr) {
        SDL_SetError("COREAUDIO_CloseAudio: AudioOutputUnitStop");
        return;
    }

    /* Remove the input callback */
    callback.inputProc = 0;
    callback.inputProcRefCon = 0;
    result = AudioUnitSetProperty(this->hidden->outputAudioUnit,
                                  kAudioUnitProperty_SetInputCallback,
                                  kAudioUnitScope_Input,
                                  0, &callback, sizeof(callback));
    if (result != noErr) {
        SDL_SetError
            ("COREAUDIO_CloseAudio: AudioUnitSetProperty (kAudioUnitProperty_SetInputCallback)");
        return;
    }

    result = CloseComponent(this->hidden->outputAudioUnit);
    if (result != noErr) {
        SDL_SetError("COREAUDIO_CloseAudio: CloseComponent");
        return;
    }

    SDL_free(this->hidden->buffer);
    SDL_free(this->hidden);
    this->hidden = NULL;
}

#define CHECK_RESULT(msg) \
    if (result != noErr) { \
        COREAUDIO_CloseAudio(this); \
        SDL_SetError("CoreAudio error (%s): %d", msg, (int) result); \
        return -1; \
    }


int
COREAUDIO_OpenAudio(_THIS, const char *devname, int iscapture)
{
    OSStatus result = noErr;
    Component comp;
    ComponentDescription desc;
    struct AudioUnitInputCallback callback;
    AudioStreamBasicDescription strdesc;
    SDL_AudioFormat test_format = SDL_FirstAudioFormat(this->spec.format);
    int valid_datatype = 0;

    /* Initialize all variables that we clean on shutdown */
    this->hidden = (struct SDL_PrivateAudioData *)
                        SDL_malloc((sizeof *this->hidden));
    if (this->hidden == NULL) {
        SDL_OutOfMemory();
        return (0);
    }
    SDL_memset(this->hidden, 0, (sizeof *this->hidden));

    /* !!! FIXME: check devname and iscapture... */

    /* Setup a AudioStreamBasicDescription with the requested format */
    memset(&strdesc, '\0', sizeof(AudioStreamBasicDescription));
    strdesc.mFormatID = kAudioFormatLinearPCM;
    strdesc.mFormatFlags = kLinearPCMFormatFlagIsPacked;
    strdesc.mChannelsPerFrame = this->spec.channels;
    strdesc.mSampleRate = this->spec.freq;
    strdesc.mFramesPerPacket = 1;

    while ((!valid_datatype) && (test_format)) {
        this->spec.format = test_format;
        /* Just a list of valid SDL formats, so people don't pass junk here. */
        switch (test_format) {
        case AUDIO_U8:
        case AUDIO_S8:
        case AUDIO_U16LSB:
        case AUDIO_S16LSB:
        case AUDIO_U16MSB:
        case AUDIO_S16MSB:
        case AUDIO_S32LSB:
        case AUDIO_S32MSB:
        case AUDIO_F32LSB:
        case AUDIO_F32MSB:
            valid_datatype = 1;
            strdesc.mBitsPerChannel = SDL_AUDIO_BITSIZE(this->spec.format);
            if (SDL_AUDIO_ISBIGENDIAN(this->spec.format))
                strdesc.mFormatFlags |= kLinearPCMFormatFlagIsBigEndian;

            if (SDL_AUDIO_ISFLOAT(this->spec.format))
                strdesc.mFormatFlags |= kLinearPCMFormatFlagIsFloat;
            else if (SDL_AUDIO_ISSIGNED(this->spec.format))
                strdesc.mFormatFlags |= kLinearPCMFormatFlagIsSignedInteger;
            break;
        }
    }

    if (!valid_datatype) {      /* shouldn't happen, but just in case... */
        SDL_SetError("Unsupported audio format");
        return 0;
    }

    strdesc.mBytesPerFrame =
        strdesc.mBitsPerChannel * strdesc.mChannelsPerFrame / 8;
    strdesc.mBytesPerPacket =
        strdesc.mBytesPerFrame * strdesc.mFramesPerPacket;

    /* Locate the default output audio unit */
    memset(&desc, '\0', sizeof(ComponentDescription));
    desc.componentType = kAudioUnitComponentType;
    desc.componentSubType = kAudioUnitSubType_Output;
    desc.componentManufacturer = kAudioUnitID_DefaultOutput;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;

    comp = FindNextComponent(NULL, &desc);
    if (comp == NULL) {
        COREAUDIO_CloseAudio(this);
        SDL_SetError
            ("Failed to start CoreAudio: FindNextComponent returned NULL");
        return 0;
    }

    /* Open & initialize the default output audio unit */
    result = OpenAComponent(comp, &this->hidden->outputAudioUnit);
    CHECK_RESULT("OpenAComponent")
        result = AudioUnitInitialize(this->hidden->outputAudioUnit);
    CHECK_RESULT("AudioUnitInitialize")
        /* Set the input format of the audio unit. */
        result = AudioUnitSetProperty(this->hidden->outputAudioUnit,
                                      kAudioUnitProperty_StreamFormat,
                                      kAudioUnitScope_Input,
                                      0, &strdesc, sizeof(strdesc));
    CHECK_RESULT("AudioUnitSetProperty (kAudioUnitProperty_StreamFormat)")
        /* Set the audio callback */
        callback.inputProc = audioCallback;
    callback.inputProcRefCon = this;
    result = AudioUnitSetProperty(this->hidden->outputAudioUnit,
                                  kAudioUnitProperty_SetInputCallback,
                                  kAudioUnitScope_Input,
                                  0, &callback, sizeof(callback));
    CHECK_RESULT("AudioUnitSetProperty (kAudioUnitProperty_SetInputCallback)")
        /* Calculate the final parameters for this audio specification */
        SDL_CalculateAudioSpec(&this->spec);

    /* Allocate a sample buffer */
    this->hidden->bufferOffset = this->hidden->bufferSize = this->spec.size;
    this->hidden->buffer = SDL_malloc(this->hidden->bufferSize);

    /* Finally, start processing of the audio unit */
    result = AudioOutputUnitStart(this->hidden->outputAudioUnit);
    CHECK_RESULT("AudioOutputUnitStart")
        /* We're running! */
        return (1);
}

/* vi: set ts=4 sw=4 expandtab: */
