//
//  SDLUIAccelerationDelegate.m
//  iPodSDL
//
//  Created by Holmes Futrell on 6/21/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "SDLUIAccelerationDelegate.h"

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

#define MAX_G_FORCE 5.0
#define MAX_SINT16 0x7FFF	

	if (x > MAX_G_FORCE) x = MAX_G_FORCE;
	else if (x < -MAX_G_FORCE) x = -MAX_G_FORCE;

	if (y > MAX_G_FORCE) y = MAX_G_FORCE;
	else if (y < -MAX_G_FORCE) y = -MAX_G_FORCE;

	if (z > MAX_G_FORCE) z = MAX_G_FORCE;
	else if (z < -MAX_G_FORCE) z = -MAX_G_FORCE;
	
	data[0] = (x / MAX_G_FORCE) * MAX_SINT16;
	data[1] = (y / MAX_G_FORCE) * MAX_SINT16;
	data[2] = (z / MAX_G_FORCE) * MAX_SINT16;

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
