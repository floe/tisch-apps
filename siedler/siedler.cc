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

int bsize = 25;
RGBATexture* empty;
RGBATexture* white;

// This class demonstrates how to react to the predefined tap gesture using Button as superclass.
class MyButton: public Button {
	public:

		MyButton( int _x = 0, int _y = 0, const char* label = "" ):
			Button( bsize,bsize,_x,_y,0.0,empty)
		{
			mycolor[0] = 1.0;
			mycolor[1] = 1.0;
			mycolor[2] = 1.0;
			mycolor[3] = 0.0;
			lbl = new Label(label,50,50,x+40,y+25,0,1,0,NULL);
			lbl->color(0,0,0,1);
			l_angle = 0;
			text = label;
		}

		void init() { parent->add(lbl); }

		void draw() {
			l_angle += 0.25;
			lbl->setangle(l_angle); //if (angle >= 2*M_PI) angle = 0;
			Button::draw();
		}

		void tap( Vector pos, int id ) { lbl->set( text ); lbl->texture( white ); }
		void release( ) { lbl->set( "" ); lbl->texture(NULL); }

		Label* lbl;
		double l_angle;
		const char* text;
};

int main( int argc, char* argv[] ) {

	std::cout << "siedler - based on libTISCH 2.0" << std::endl;
	std::cout << "(c) 2012 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	// create a new window: width, height, title, use_mouse
	Window* win = new Window( 800, 600, "siedler", (argc > 1) );
	win->texture( new RGBATexture("siedler.png") );

	empty = new RGBATexture("empty.png");
	white = new RGBATexture("white.png");

	int dx = -29;
	int dy = 35;

	MyButton* btn1 = new MyButton(   0+dx,-33.5+dy, "25%" ); win->add( btn1 ); btn1->init();
	MyButton* btn2 = new MyButton(   0+dx, 33.5+dy, "28%" ); win->add( btn2 ); btn2->init();

	MyButton* btn3 = new MyButton(  29+dx,-17.5+dy, "17%" ); win->add( btn3 ); btn3->init();
	MyButton* btn4 = new MyButton(  29+dx, 17.5+dy, "22%" ); win->add( btn4 ); btn4->init();

	MyButton* btn5 = new MyButton( -29+dx,-17.5+dy, "33%" ); win->add( btn5 ); btn5->init();
	MyButton* btn6 = new MyButton( -29+dx, 17.5+dy, "33%" ); win->add( btn6 ); btn6->init();

	win->update();
	win->run();
}

