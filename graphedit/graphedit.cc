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
class Node;

struct Edge {
	Node* n1;
	Node* n2;
};

std::vector<Node*> node_list;
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


class Node: public Tile {
	public:

		Node( int _x, int _y, RGBATexture* _tex ):
		Tile( 100, 100, _x, _y, 0, _tex, TISCH_TILE_MOVE|TISCH_TILE_SLIDE|TISCH_TILE_BOUNCE) {
			shadow = false;

			for(std::vector<Gesture>::iterator it = region.gestures.begin(); it != region.gestures.end(); it++) {
				if(it->name() == "move") { region.gestures.erase(it); break; }
			}

			Gesture move( "move", GESTURE_FLAGS_STICKY );

			move.push_back(new Motion());
			std::vector<int> b1; b1.push_back( 1 ); b1.push_back( 1 );
			BlobCount* bcnt1 = new BlobCount(1<<INPUT_TYPE_FINGER);
			bcnt1->bounds( b1 );
			move.push_back( bcnt1 );
			region.gestures.push_back( move );	

			Gesture link( "link", GESTURE_FLAGS_STICKY );

			link.push_back(new Motion());
			std::vector<int> b2; b2.push_back( 2 ); b2.push_back( 2 );
			BlobCount* bcnt2 = new BlobCount(1<<INPUT_TYPE_FINGER);
			bcnt2->bounds( b2 );
			link.push_back( bcnt2 );
			region.gestures.push_back( link );	

			lx = x; ly = y; do_link = 0;
		}

		void tap( Vector pos, int id ) {
			lx = x;
			ly = y;
		}

		void release( ) {
			if (do_link) {
				std::cout << "testing: " << lx << " " << ly << std::endl;
				for (std::vector<Node*>::iterator node = node_list.begin(); node != node_list.end(); node++) {
					std::cout << "node: " << (*node)->x << " " << (*node)->y << std::endl;
					if ((*node)->inside( lx, ly )) {
						std::cout << "inside " << std::endl;
						CloseButton* tmp = new CloseButton( 20, 20, 100, 100 );
						tmp->region.flags( REGION_FLAGS_VOLATILE | (1 << INPUT_TYPE_FINGER) );
						parent->add( tmp );
						edge_list[tmp] = { this, *node };
						break;
					}
				}
			}
			lx = x;
			ly = y;
			do_link = 0;
		}

		void draw() {

			if (do_link) {
				glLineWidth(2.0);
				glColor4f(0.0,1.0,0.0,1.0);

				glBegin(GL_LINES);
					glVertex2f(x,y);
					glVertex2f(lx,ly);
				glEnd();
			}

			enter();

			Widget::paint();

			leave();
		}

		bool inside( int _x, int _y ) {
			double dx = x - _x;
			double dy = y - _y;
			return (sqrt(dx*dx+dy*dy)<50);
		}


		void action( Gesture* gst ) {
			if (gst->name() == "link") {
				FeatureBase* f = (*gst)[0];
				Motion* m = dynamic_cast<Motion*>(f);
				Vector tmp = m->result();
				transform(tmp,0,1);
				lx += tmp.x;
				ly += tmp.y;
				do_link = 1;
			} else Tile::action( gst );
		}

		int lx, ly, do_link;
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

	std::cout << "graphedit - libTISCH 2.0 widget layer demo" << std::endl;
	std::cout << "(c) 2011 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	int mouse = ((argc > 1) && (std::string("-m") == argv[1]));
	GraphWin* win = new GraphWin( 800, 480, "Graph", mouse );
	win->texture(0);

	srandom(45890);

	RGBATexture* tmp = new RGBATexture( TISCH_PREFIX "Dial.png" );

	for (int i = 0; i < 20; i++) {
		Node* foo = new Node(
			(int)(((double)random()/(double)RAND_MAX)*700-350),
			(int)(((double)random()/(double)RAND_MAX)*450-225),
			tmp
		);
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

