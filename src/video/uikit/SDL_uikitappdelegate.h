//
//  SDLUIKitDelegate.h
//  iPodSDL
//
//  Created by Holmes Futrell on 5/29/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SDL_uikitopenglview.h"

@interface SDLUIKitDelegate : NSObject<UIApplicationDelegate> {
	UIWindow *window;
	SDL_uikitopenglview *view;
}

@end
