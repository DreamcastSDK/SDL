//
//  SDLUIAccelerationDelegate.h
//  iPodSDL
//
//  Created by Holmes Futrell on 6/21/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SDL_stdinc.h"

@interface SDLUIAccelerationDelegate: NSObject <UIAccelerometerDelegate> {

	UIAccelerationValue x, y, z;
	//NSTimeInterval timestamp;
	BOOL isRunning;
	BOOL hasNewData;
	
}

+(SDLUIAccelerationDelegate *)sharedDelegate;
-(void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration;
-(void)getLastOrientation:(Sint16 *)data;
-(void)startup;
-(void)shutdown;
-(BOOL)isRunning;
-(BOOL)hasNewData;
-(void)setHasNewData:(BOOL)value;

@end
