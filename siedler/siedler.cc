/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2012 by Florian Echtler <floe@butterbrot.org>    *
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

// This class demonstrates how to react to the predefined tap gesture using Button as superclass.
/*class MyButton: public Button {
	public:

		MyButton( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0 ):
			Button( _w,_h,_x,_y,angle,_tex) { }

		void tap( Vector pos, int id ) { lbl->set( " tap!" ); }
		void release( ) { lbl->set( " Button" ); }

};*/


int main( int argc, char* argv[] ) {

	std::cout << "siedler - based on libTISCH 2.0" << std::endl;
	std::cout << "(c) 2012 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	// create a new window: width, height, title, use_mouse
	Window* win = new Window( 800, 600, "siedler", (argc > 1) );
	win->texture( new RGBATexture("siedler.png") );

	Button* btn1 = new Button( 10,10,   0,-33.5 ); win->add( btn1 );
	Button* btn2 = new Button( 10,10,   0, 33.5 ); win->add( btn2 );

	Button* btn3 = new Button( 10,10,  29,-17.5 ); win->add( btn3 );
	Button* btn4 = new Button( 10,10,  29, 17.5 ); win->add( btn4 );

	Button* btn5 = new Button( 10,10, -29,-17.5 ); win->add( btn5 );
	Button* btn6 = new Button( 10,10, -29, 17.5 ); win->add( btn6 );

	win->update();
	win->run();
}

