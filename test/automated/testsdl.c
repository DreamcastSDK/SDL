/*
 * SDL test suite framework code.
 *
 * Written by Edgar Simo "bobbens"
 *
 * Released under Public Domain.
 */


#include "SDL_at.h"

#include "platform/platform.h"
#include "rwops/rwops.h"
#include "surface/surface.h"
#include "render/render.h"

#include <stdio.h> /* printf */
#include <stdlib.h> /* exit */
#include <unistd.h> /* getopt */
#include <getopt.h> /* getopt_long */


/*
 * Prototypes.
 */
static void print_usage( const char *name );
static void parse_options( int argc, char *argv[] );


/**
 * @brief Displays program usage.
 */
static void print_usage( const char *name )
{
   printf("Usage: %s [OPTIONS]\n", name);
   printf("Options are:\n");
   printf("   -v, --verbose   increases verbosity level by 1 for each -v\n");
   printf("   -q, --quiet     only displays errors\n");
   printf("   -h, --help      display this message and exit\n");
}


/**
 * @brief Handles the options.
 */
static void parse_options( int argc, char *argv[] )
{
   static struct option long_options[] = {
      { "verbose", no_argument, 0, 'v' },
      { "quiet", no_argument, 0, 'q' },
      { "help", no_argument, 0, 'h' },
      {NULL,0,0,0}
   };
   int option_index = 0;
   int c = 0;
   int i;

   /* Iterate over options. */
   while ((c = getopt_long( argc, argv,
               "vqh",
               long_options, &option_index)) != -1) {

      /* Handle options. */
      switch (c) {

         /* Verbosity. */
         case 'v':
            SDL_ATgeti( SDL_AT_VERBOSE, &i );
            SDL_ATseti( SDL_AT_VERBOSE, i+1 );
            break;

         /* Quiet. */
         case 'q':
            SDL_ATseti( SDL_AT_QUIET, 1 );
            break;

         /* Help. */
         case 'h':
            print_usage( argv[0] );
            exit(EXIT_SUCCESS);
      }
   }

}


/**
 * @brief Main entry point.
 */
int main( int argc, char *argv[] )
{
   parse_options( argc, argv );

   test_platform();
   test_rwops();
   test_surface();
   test_render();

   return 0;
}

