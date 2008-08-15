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

#import "SDL_uikitview.h"

#if SDL_IPHONE_KEYBOARD
#import "SDL_keyboard_c.h"
#import "keyinfotable.h"
#import "SDL_uikitappdelegate.h"
#import "SDL_uikitwindow.h"
#endif

@implementation SDL_uikitview

- (void)dealloc {
#if SDL_IPHONE_KEYBOARD
	[textField release];
#endif
	[super dealloc];
}

- (id)initWithFrame:(CGRect)frame {

	self = [super initWithFrame: frame];
	
#if SDL_IPHONE_KEYBOARD
	[self initializeKeyboard];
#endif	

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
		at once, or perhaps in other circumstances.  Usually (it seems)
		all active touches are canceled.
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

/*
	---- Keyboard related functionality below this line ----
*/
#if SDL_IPHONE_KEYBOARD

- (BOOL)keyboardVisible {
	return keyboardVisible;
}

/* UITextFieldDelegate related methods */
- (void)initializeKeyboard {
		
	textField = [[UITextField alloc] initWithFrame: CGRectZero];
	textField.delegate = self;
	/* placeholder so there is something to delete! */
	textField.text = @" ";	
	
	/* set UITextInputTrait properties, mostly to defaults */
	textField.autocapitalizationType = UITextAutocapitalizationTypeNone;
	textField.autocorrectionType = UITextAutocorrectionTypeNo;
	textField.enablesReturnKeyAutomatically = NO;
	textField.keyboardAppearance = UIKeyboardAppearanceDefault;
	textField.keyboardType = UIKeyboardTypeDefault;
	textField.returnKeyType = UIReturnKeyDefault;
	textField.secureTextEntry = NO;	
	
	textField.hidden = YES;
	keyboardVisible = NO;
	[self addSubview: textField];
	
	/*
	 SDL makes a copy of our keyboard.
	 */
	
	SDL_Keyboard keyboard;
	SDL_zero(keyboard);
	//data->keyboard = SDL_AddKeyboard(&keyboard, -1);
	/* 
	 We'll need to delete this keyboard ...
	 */
	SDL_AddKeyboard(&keyboard, 0);
	SDLKey keymap[SDL_NUM_SCANCODES];
	SDL_GetDefaultKeymap(keymap);
	SDL_SetKeymap(0, 0, keymap, SDL_NUM_SCANCODES);
	
}

- (void)showKeyboard {
	keyboardVisible = YES;
	[textField becomeFirstResponder];
}

- (void)hideKeyboard {
	keyboardVisible = NO;
	[textField resignFirstResponder];
}

- (BOOL)textField:(UITextField *)_textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string {
	
	if ([string length] == 0) {
		/* it wants to replace text with nothing, ie a delete */
		SDL_SendKeyboardKey( 0, SDL_PRESSED, SDL_SCANCODE_DELETE);
		SDL_SendKeyboardKey( 0, SDL_RELEASED, SDL_SCANCODE_DELETE);
	}
	else {
		
		int i;
		for (i=0; i<[string length]; i++) {
			
			unichar c = [string characterAtIndex: i];
			
			Uint16 mod = 0;
			SDL_scancode code;
			
			if (c < 127) {
				code = unicharToUIKeyInfoTable[c].code;
				mod  = unicharToUIKeyInfoTable[c].mod;
			}
			else {
				code = SDL_SCANCODE_UNKNOWN;
				mod = 0;
			}
			
			if (mod & KMOD_SHIFT) {
				SDL_SendKeyboardKey( 0, SDL_PRESSED, SDL_SCANCODE_LSHIFT);
			}
			SDL_SendKeyboardKey( 0, SDL_PRESSED, code);
			SDL_SendKeyboardKey( 0, SDL_RELEASED, code);
			if (mod & KMOD_SHIFT) {
				SDL_SendKeyboardKey( 0, SDL_RELEASED, SDL_SCANCODE_LSHIFT);
			}			
			
		}
		
	}
	return NO; /* don't allow the edit(!) */
}

/* Terminates the editing session */
- (BOOL)textFieldShouldReturn:(UITextField*)_textField {
	[self hideKeyboard];
	return YES;
}

#endif

@end



/* iPhone keyboard addition functions */
#if SDL_IPHONE_KEYBOARD

int SDL_iPhoneKeyboardShow(SDL_WindowID windowID) {
	
	SDL_Window *window = SDL_GetWindowFromID(windowID);
	SDL_WindowData *data;
	SDL_uikitview *view;
	
	if (NULL == window) {
		SDL_SetError("Window does not exist");
		return -1;
	}
	
	data = (SDL_WindowData *)window->driverdata;
	view = data->view;
	
	if (nil == view) {
		SDL_SetError("Window has no view");
		return -1;
	}
	else {
		[view showKeyboard];
		return 0;
	}
}

int SDL_iPhoneKeyboardHide(SDL_WindowID windowID) {
	
	SDL_Window *window = SDL_GetWindowFromID(windowID);
	SDL_WindowData *data;
	SDL_uikitview *view;
	
	if (NULL == window) {
		SDL_SetError("Window does not exist");
		return -1;
	}	
	
	data = (SDL_WindowData *)window->driverdata;
	view = data->view;
	
	if (NULL == view) {
		SDL_SetError("Window has no view");
		return -1;
	}
	else {
		[view hideKeyboard];
		return 0;
	}
}

SDL_bool SDL_iPhoneKeyboardIsShown(SDL_WindowID windowID) {
	
	SDL_Window *window = SDL_GetWindowFromID(windowID);
	SDL_WindowData *data;
	SDL_uikitview *view;
	
	if (NULL == window) {
		SDL_SetError("Window does not exist");
		return -1;
	}	
	
	data = (SDL_WindowData *)window->driverdata;
	view = data->view;
	
	if (NULL == view) {
		SDL_SetError("Window has no view");
		return 0;
	}
	else {
		return view.keyboardVisible;
	}
}

int SDL_iPhoneKeyboardToggle(SDL_WindowID windowID) {
	
	SDL_Window *window = SDL_GetWindowFromID(windowID);
	SDL_WindowData *data;
	SDL_uikitview *view;
	
	if (NULL == window) {
		SDL_SetError("Window does not exist");
		return -1;
	}	
	
	data = (SDL_WindowData *)window->driverdata;
	view = data->view;
	
	if (NULL == view) {
		SDL_SetError("Window has no view");
		return -1;
	}
	else {
		if (SDL_iPhoneKeyboardIsShown(windowID)) {
			SDL_iPhoneKeyboardHide(windowID);
		}
		else {
			SDL_iPhoneKeyboardShow(windowID);
		}
		return 0;
	}
}

#else

int SDL_iPhoneKeyboardShow(SDL_WindowID windowID) {
	SDL_SetError("Not compiled with keyboard support");
	return -1;
}

int SDL_iPhoneKeyboardHide(SDL_WindowID windowID) {
	SDL_SetError("Not compiled with keyboard support");
	return -1;
}

SDL_bool SDL_iPhoneKeyboardIsShown(SDL_WindowID windowID) {
	return 0;
}

int SDL_iPhoneKeyboardToggle(SDL_WindowID windowID) {
	SDL_SetError("Not compiled with keyboard support");
	return -1;
}


#endif /* SDL_IPHONE_KEYBOARD */