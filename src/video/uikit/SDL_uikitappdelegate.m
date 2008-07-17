//
//  SDLUIKitDelegate.m
//  iPodSDL
//
//  Created by Holmes Futrell on 5/29/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "SDL_uikitappdelegate.h"
#import "SDL_uikitopenglview.h"
#import <pthread.h>
#import "SDL_events_c.h"
#import "jump.h"

#ifdef main
#undef main
#endif

UIWindow *uikitWindow=nil;
SDL_uikitopenglview *uikitEAGLView=nil;

extern int SDL_main(int argc, char *argv[]);
static int forward_argc;
static char **forward_argv;

int main(int argc, char **argv) {

	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	/* store arguments */
	forward_argc = argc;
	forward_argv = (char **)malloc(argc * sizeof(char *));
	int i;
	for (i=0; i<argc; i++) {
		forward_argv[i] = malloc( (strlen(argv[i])+1) * sizeof(char));
		strcpy(forward_argv[i], argv[i]);
	}

	UIApplicationMain(argc, argv, NULL, @"SDLUIKitDelegate");
	
	[pool release];
	
}

@implementation SDLUIKitDelegate

- (void)applicationDidFinishLaunching:(UIApplication *)application {
			
	/* Set working directory to resource path */
	[[NSFileManager defaultManager] changeCurrentDirectoryPath: [[NSBundle mainBundle] resourcePath]];
	
	/* run the user's application, passing argc and argv */
	int exit_status = SDL_main(forward_argc, forward_argv);
	
	/* free the memory we used to hold copies of argc and argv */
	int i;
	for (i=0; i<forward_argc; i++) {
		free(forward_argv[i]);
	}
	free(forward_argv);	
		
	/* exit, passing the return status from the user's application */
	exit(exit_status);
		
}

- (void)applicationWillTerminate:(UIApplication *)application {
	
	SDL_SendQuit();
	longjmp(*(jump_env()), 1); // hack to prevent automatic termination
	
}

-(void)dealloc {
	[uikitWindow release];
	[uikitEAGLView release]; 
	[super dealloc];
}

@end
