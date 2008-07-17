//
//  SDL_uikitview.m
//  iPodSDL
//
//  Created by Holmes Futrell on 6/23/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "SDL_uikitview.h"

@implementation SDL_uikitview

- (void)dealloc {
	[super dealloc];
}

- (id)initWithFrame:(CGRect)frame {

	self = [super initWithFrame: frame];
	
	int i;
	for (i=0; i<MAX_SIMULTANEOUS_TOUCHES; i++) {
		mice[i].driverdata = NULL;
		SDL_AddMouse(&(mice[i]), i);
	}
	self.multipleTouchEnabled = YES;
		
	return self;

}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {

	NSEnumerator *enumerator = [touches objectEnumerator];
	UITouch *touch=(UITouch*)[enumerator nextObject];
	
	// associate touches with mice, so long as we have slots
	int i;
	int found = 0;
	for(i=0; touch && i < MAX_SIMULTANEOUS_TOUCHES; i++) {
	
		// check if this mouse is already tracking a touch
		if (mice[i].driverdata != NULL) {
			continue;
		}
		
		found = 1;
		
		int oldMouse = SDL_SelectMouse(-1);
		SDL_SelectMouse(i);
		CGPoint locationInView = [touch locationInView: self];
		mice[i].driverdata = [touch retain];
		SDL_SendMouseMotion(i, 0, locationInView.x, locationInView.y);
		SDL_SendMouseButton(i, SDL_PRESSED, SDL_BUTTON_LEFT);
		SDL_GetRelativeMouseState(NULL, NULL);
		touch = (UITouch*)[enumerator nextObject]; 
		
		SDL_SelectMouse(oldMouse);
		
	}	
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	
	NSEnumerator *enumerator = [touches objectEnumerator];
	UITouch *touch=nil;
	
	while(touch = (UITouch *)[enumerator nextObject]) {
		int i, found = NO;
		for (i=0; i<MAX_SIMULTANEOUS_TOUCHES && !found; i++) {
			if (mice[i].driverdata == touch) {
				[(UITouch*)(mice[i].driverdata) release];
				mice[i].driverdata = NULL;
				SDL_SendMouseButton(i, SDL_RELEASED, SDL_BUTTON_LEFT);

				found = YES;
			}
		}
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	/*
		this can happen if the user puts more than 5 touches on the screen
		at once, or perhaps in other circumstances.  Usually all active
		touches are canceled.
	*/
	[self touchesEnded: touches withEvent: event];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	
	NSEnumerator *enumerator = [touches objectEnumerator];
	UITouch *touch=nil;
	
	while(touch = (UITouch *)[enumerator nextObject]) {
		int i, found = NO;
		for (i=0; i<MAX_SIMULTANEOUS_TOUCHES && !found; i++) {
			if (mice[i].driverdata == touch) {
				CGPoint locationInView = [touch locationInView: self];
				SDL_SendMouseMotion(i, 0, locationInView.x, locationInView.y);
				found = YES;
			}
		}
	}
}

@end
