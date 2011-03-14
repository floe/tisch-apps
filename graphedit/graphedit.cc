/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdlib.h>
#include <nanolibc.h>

#include "Window.h"
#include "Tile.h"
#include "Textbox.h"
#include <Motion.h>
#include <BlobCount.h>

class CloseButton;

struct Edge {
	Tile* n1;
	Tile* n2;
};

std::vector<Tile*> node_list;
std::map<CloseButton*,Edge> edge_list;


class CloseButton: public Button {
	public:

		CloseButton( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0 ):
			Button( _w,_h,_x,_y,angle,_tex) { }

		void tap( Vector pos, int id ) {
			edge_list.erase( this );
			parent->remove( this );
			delete this;
		}
};



class GraphWin: public Window {

public:

	GraphWin( int w, int h, std::string title, int use_mouse ): Window(w,h,title,use_mouse) {
		shadow = false;
	}

	void draw() {

		enter();

		Widget::paint();

		glLineWidth(2.0);
		glColor4f(0.0,0.0,1.0,1.0);

		for (std::map<CloseButton*,Edge>::iterator it = edge_list.begin(); it != edge_list.end(); it++) {

			double x1 = it->second.n1->x; double x2 = it->second.n2->x;
			double y1 = it->second.n1->y; double y2 = it->second.n2->y;

			glBegin(GL_LINES);
				glVertex2f(x1,y1);
				glVertex2f(x2,y2);
			glEnd();

			CloseButton* tmp = it->first;
			tmp->x = (x1+x2)/2.0;
			tmp->y = (y1+y2)/2.0;
		}

		MasterContainer::paint();

		leave();
	}
};


int main( int argc, char* argv[] ) {

	std::cout << "graph - libTISCH 1.1 widget layer demo" << std::endl;
	std::cout << "(c) 2010 by Florian Echtler <echtler@in.tum.de>" << std::endl;

	int mouse = ((argc > 1) && (std::string("-m") == argv[1]));
	GraphWin* win = new GraphWin( 800, 480, "Graph", mouse );
	win->texture(0);

	srandom(45890);

	RGBATexture* tmp = new RGBATexture( TISCH_PREFIX "Dial.png" );

	for (int i = 0; i < 20; i++) {
		Tile* foo = new Tile( 50, 50,
			(int)(((double)random()/(double)RAND_MAX)*700-350),
			(int)(((double)random()/(double)RAND_MAX)*450-225),
			0, tmp, TISCH_TILE_MOVE|TISCH_TILE_SLIDE|TISCH_TILE_BOUNCE
		);
		foo->shadow = false;
		win->add( foo );
		node_list.push_back(foo);
	}
	
	for (int i = 0; i < 20; i++) {
		int pair = (int)(((double)random()/(double)RAND_MAX)*19);
		if (pair >= 0) { 
			CloseButton* tmp = new CloseButton( 20, 20, 100, 100 );
			tmp->region.flags( REGION_FLAGS_VOLATILE | (1 << INPUT_TYPE_FINGER) );
			win->add( tmp );
			edge_list[tmp] = { node_list[i], node_list[pair] };
		}
	}

	win->update();
	win->run();
}

