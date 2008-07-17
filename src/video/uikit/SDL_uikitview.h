//
//  SDL_uikitview.h
//  iPodSDL
//
//  Created by Holmes Futrell on 6/23/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "SDL_stdinc.h"
#include "SDL_mouse.h"
#include "SDL_mouse_c.h"
#include "SDL_events.h"

#define MAX_SIMULTANEOUS_TOUCHES 5

@interface SDL_uikitview : UIView {
		
	SDL_Mouse mice[MAX_SIMULTANEOUS_TOUCHES];
	
}
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;

@end
