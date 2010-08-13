#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>

Display *display;  /* Server or display to which a connection will be made */
int screen;        /* Screen number to use on that display */

int main(argc, argv)
int argc; char **argv;
{
	int dev = 11;
	//int axes[2] = { 640, 480 };
	int axes[2] = { 10, 1040 };

  /* Attempt to connect to the server and use the default screen */
  if ((display = XOpenDisplay(NULL)) == NULL) {
    perror("Can't connect to server");
    exit(1);
  }
  screen = DefaultScreen(display);
	//DisplayWidth(display,screen)
	//DisplayHeight(display,screen)

	// probably a bug, can be removed later
	//XIWarpPointer(display, dev, None, RootWindow(display, screen), 0, 0, 0, 0, axes[0], axes[1] );
	
	XDevice xdev = { dev, 0, 0 };
	XTestFakeDeviceMotionEvent( display, &xdev, False, 0, axes, 2, CurrentTime );
	XTestFakeDeviceButtonEvent( display, &xdev, 1,  True, axes, 2, CurrentTime );
	XTestFakeDeviceButtonEvent( display, &xdev, 1, False, axes, 2, CurrentTime );

	XFlush(display);

	return 0;
}

// build with gcc -lX11 -lXi -lXtst -Wall xiwarptest.c -o xtest
