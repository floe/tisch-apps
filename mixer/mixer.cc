/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdlib.h>

#include "Slider.h"
#include "Window.h"
#include "Textbox.h"
#include "Checkbox.h"
#include "Tile.h"
#include "Dial.h"
#include "Label.h"

Label* lbl;

// This class demonstrates how to react to the predefined tap gesture using Button as superclass.
class MixerBlock: public Container {
	public:

		MixerBlock( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0 ):
			Container( _w,_h,_x,_y,angle,_tex,0xFF) {
			
			add( new Slider( 250,50, 0,70, 90 ) );
			add( new Label( "Volume", 100,20, 0,-70, 0, 1 ) );

			add( new Checkbox( 20,20, -30,-105 ) );
			add( new Label( "Mute", 100,20, 40,-105 ) );

			Dial* d = new Dial( 60, 0,-150, 0 ); d->set(M_PI,0,2*M_PI);
			add( d );
			add( new Label( "Balance", 100,20, 0,-180 ) );
		}

		// example code for a "close" button:
		/*Container* foo = parent;
			foo->remove(this);
			delete foo; 
			delete this;*/
};


int main( int argc, char* argv[] ) {

	// create a new window: width, height, title, use_mouse
	Window* win = new Window( 800, 480, "Audio Mixer Demo", (argc > 1) );
	win->texture( 0 );

	MixerBlock* foo = new MixerBlock( 150, 400, 0, 0, 0, 0 );
	win->add( foo );

	foo = new MixerBlock( 150, 400, 150, 0, 0, 0 );
	win->add( foo );

	win->update();
	win->run();
}

