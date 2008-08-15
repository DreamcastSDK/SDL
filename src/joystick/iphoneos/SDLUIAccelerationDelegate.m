//
//  SDLUIAccelerationDelegate.m
//  iPodSDL
//
//  Created by Holmes Futrell on 6/21/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "SDLUIAccelerationDelegate.h"
#import "../../../include/SDL_config_iphoneos.h"

static SDLUIAccelerationDelegate *sharedDelegate=nil;

@implementation SDLUIAccelerationDelegate

+(SDLUIAccelerationDelegate *)sharedDelegate {
	if (sharedDelegate == nil) {
		sharedDelegate = [[SDLUIAccelerationDelegate alloc] init];
	}
	return sharedDelegate;
}

-(void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration {
		
	x = acceleration.x;
	y = acceleration.y;
	z = acceleration.z;
	
	hasNewData = YES;
	//timestamp = acceleration.timestamp;
	
}

-(void)getLastOrientation:(Sint16 *)data {

	#define MAX_SINT16 0x7FFF	

	if (x > SDL_IPHONE_MAX_GFORCE) x = SDL_IPHONE_MAX_GFORCE;
	else if (x < -SDL_IPHONE_MAX_GFORCE) x = -SDL_IPHONE_MAX_GFORCE;

	if (y > SDL_IPHONE_MAX_GFORCE) y = SDL_IPHONE_MAX_GFORCE;
	else if (y < -SDL_IPHONE_MAX_GFORCE) y = -SDL_IPHONE_MAX_GFORCE;

	if (z > SDL_IPHONE_MAX_GFORCE) z = SDL_IPHONE_MAX_GFORCE;
	else if (z < -SDL_IPHONE_MAX_GFORCE) z = -SDL_IPHONE_MAX_GFORCE;
	
	data[0] = (x / SDL_IPHONE_MAX_GFORCE) * MAX_SINT16;
	data[1] = (y / SDL_IPHONE_MAX_GFORCE) * MAX_SINT16;
	data[2] = (z / SDL_IPHONE_MAX_GFORCE) * MAX_SINT16;

}

-(id)init {

	self = [super init];
	x = y = z = 0.0;
	return self;
	
}

-(void)dealloc {
	sharedDelegate = nil;
	[super dealloc];
}

-(void)startup {
	[UIAccelerometer sharedAccelerometer].delegate = self;
	isRunning = YES;
}

-(void)shutdown {
	[UIAccelerometer sharedAccelerometer].delegate = nil;
	isRunning = NO;
}

-(BOOL)isRunning {
	return isRunning;
}

-(BOOL)hasNewData {
	return hasNewData;
}

-(void)setHasNewData:(BOOL)value {
	hasNewData = value;
}

@end
