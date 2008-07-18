==============================================================================
Building the Simple DirectMedia Layer with iPhone OS 2.0
==============================================================================

These instructions are for people who wish to use SDL to compile applications for iPhone OS 2.0.

To build SDL, simply open the XCode Project (XCodeiPhoneOS/SDLiPhoneOS/SDLiPhoneOS.xcodeproj), select the target, and hit 'build'.  You can also build in a CLI environment using the xcodebuild command line tool, if you wish. 

There are three build targets:
- StaticLibiPhoneOS:
	Build SDL as a statically linked (armv6) library for iPhone OS 2.0.
- StaticLibSimulator:
	Build SDL as a statically linked (x86) library for the iPhone Simulator
- Template:
	Package a project template together with freshly built libraries and copied headers.
  	The template includes proper references to the SDL library, skeleton code for a basic SDL program, and placeholder graphics for the application icon and startup screen.

==============================================================================
Using the Simple DirectMedia Layer with iPhone OS 2.0
==============================================================================

Here's probably the easiest method:
1.  Build the iPhone SDL Application template.
1.  Install the iPhone SDL Application template (recommended directory is /Developer/Platforms/iPhoneOS.platform/Developer/Library/XCode/Project Templates/SDL Application/)
2.  Start a new project using the template (SDL will be automatically included in your project)

Here's a more manual method:
1.  Create a new iPhone project
2.  Build the SDL libraries (libSDLiPhoneOS.a and libSDLSimulator.a) and include them in your project.  XCode will ignore the library that is not currently of the correct architecture, hence your app will work both on iPhone and in the iPhone Simulator.
3.  include the SDL header files in your project.
4.  Remove main.m and replace it with a new main.m (or main.c) which is coded like a normal SDL program.  To replace main.m with a main.c, you must tell XCode not to use the project prefix file, which includes references to Cocoa Touch.
==============================================================================
Notes -- reading and writing files
==============================================================================

Each application installed on iPhone resides in its own sandbox environment, including its own Application Home directory.  For each installed application, the system generates a unique name for the application home directory, which appears as a long, incomprehensible string of numbers.

Once your application is installed, the directory tree looks like:

MySDLApp Home/
	MySDLApp.app
	Documents/
	Library/
		Preferences/
	tmp/

When your SDL based iPhone application starts up, it sets the working directory to the main bundle (MySDLApp Home/MySDLApp.app), where your application resources are stored.  You cannot write to this directory, however -- instead, I advise you to write document files to "../Documents/" and preferences to "../Library/Preferences".  

More information on this subject is available here:
http://developer.apple.com/iphone/library/documentation/iPhone/Conceptual/iPhoneOSProgrammingGuide/ApplicationEnvironment/chapter_6_section_3.html#//apple_ref/doc/uid/TP40007072-CH7-SW21
