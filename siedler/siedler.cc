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


/*
 * TODO:
 *
 * add labels with number in center of each hexagon (how to handle desert?)
 * calculate percentages, add labels to each crossing
 *
 */

int bsize = 15;
RGBATexture* empty;

// This class demonstrates how to react to the predefined tap gesture using Button as superclass.
class MyButton: public Button {
	public:

		MyButton( int _x = 0, int _y = 0 ):
			Button( bsize,bsize,_x,_y,0.0,empty)
		{
			mycolor[0] = 1.0;
			mycolor[1] = 1.0;
			mycolor[2] = 1.0;
			mycolor[3] = 0.0;
			lbl = new Label(" ",50,20,x+25,y+25);
		}

		void init() { parent->add(lbl); }

		void tap( Vector pos, int id ) { lbl->set( "foo!" ); }
		void release( ) { lbl->set( " " ); }

		Label* lbl;
};

int main( int argc, char* argv[] ) {

	std::cout << "siedler - based on libTISCH 2.0" << std::endl;
	std::cout << "(c) 2012 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	empty = new RGBATexture("empty.png");

	// create a new window: width, height, title, use_mouse
	Window* win = new Window( 800, 600, "siedler", (argc > 1) );
	win->texture( new RGBATexture("siedler.png") );

	MyButton* btn1 = new MyButton(   0,-33.5 ); win->add( btn1 ); btn1->init();
	MyButton* btn2 = new MyButton(   0, 33.5 ); win->add( btn2 ); btn2->init();

	MyButton* btn3 = new MyButton(  29,-17.5 ); win->add( btn3 ); btn3->init();
	MyButton* btn4 = new MyButton(  29, 17.5 ); win->add( btn4 ); btn4->init();

	MyButton* btn5 = new MyButton( -29,-17.5 ); win->add( btn5 ); btn5->init();
	MyButton* btn6 = new MyButton( -29, 17.5 ); win->add( btn6 ); btn6->init();

	win->update();
	win->run();
}

