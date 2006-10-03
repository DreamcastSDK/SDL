#include "SDL.h"

static void print_devices(int iscapture)
{
    const char *typestr = ((iscapture) ? "capture" : "output");
    int n = SDL_GetNumAudioDevices(iscapture);

    if (n == 0)
        printf("No %s devices.\n\n", typestr);
    else
    {
        int i;
        printf("%s devices:\n", typestr);
        for (i = 0; i < n; i++) {
            printf("  %s\n", SDL_GetAudioDevice(i, iscapture));
        }
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    /* Print available audio drivers */
    int n = SDL_GetNumAudioDrivers();
    if (n == 0) {
        printf("No built-in audio drivers\n\n");
    } else {
        printf("Built-in audio drivers:\n");
        int i;
        for (i = 0; i < n; ++i) {
            printf("  %s\n", SDL_GetAudioDriver(i));
        }
        printf("\n");
    }

    /* Load the SDL library */
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        return (1);
    }

    printf("Using audio driver: %s\n\n", SDL_GetCurrentAudioDriver());

    print_devices(0);
    print_devices(1);

    SDL_Quit();
    return 0;
}

