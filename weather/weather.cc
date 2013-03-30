/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdlib.h>
#include <nanolibc.h>

#include "Window.h"
#include "Label.h"
#include "Container.h"
#include <Motion.h>

double g_angle = M_PI/2;
int labelwidth = 240;

class SlideContainer: public Container {

	public:

		SlideContainer( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0xFF):
			Container( _w,_h,_x,_y,angle,_tex,mode) 
		{
			region.flags( INPUT_TYPE_ANY | REGION_FLAGS_VOLATILE );
			color(0,0,0,1);
			texture(NULL);
		}

		void draw() {
			for (std::deque<Widget*>::iterator it = widgets.begin(); it != widgets.end(); it++) {
				(*it)->setangle(g_angle);
			}
			g_angle += 0.005;
			if (g_angle >= 2*M_PI) g_angle = 0;
			Container::draw();
		}

		void apply( Vector delta ) {
			x += delta.x;
			vel.x = delta.x;
			if (fabs(vel.x) < 0.1) slide = 0;
			if (x >  w/2) x =  w/2;
			if (x < -w/2) x = -w/2;
		}
};


int main( int argc, char* argv[] ) {

	std::cout << "weather - libTISCH 3.0 widget layer demo" << std::endl;
	std::cout << "(c) 2011 by Florian Echtler <floe@butterbrot.org>" << std::endl;

	int mouse = ((argc > 1) && (std::string("-m") == argv[1]));
	if (argc <= mouse+1) {
		std::cout << "\nUsage: weather [-m] pic1.png pic2.png ..." << std::endl;
		return 1;
	}

	Window* win = new Window( 800, 600, "weather", mouse );
	win->texture(0);
	win->color(0,0,0,1);

	double total_width = 0;
	std::map<RGBATexture*,double> texlist;

	// load the textures
	for (int i = mouse+1; i < argc; i++) {
		RGBATexture* tmp = new RGBATexture( argv[i] );
		double new_width = tmp->width(1)*labelwidth/tmp->height(1);
		total_width += new_width+100;
		texlist[tmp] = new_width;
	}

	// create appropriately sized container
	SlideContainer* cont = new SlideContainer( total_width,labelwidth, total_width/2,0, 0,0, TISCH_TILE_MOVE | TISCH_TILE_SLIDE );
	cont->shadow = false;
	double currpos = -total_width/2;

	// create labels, add to container
	for (std::map<RGBATexture*,double>::iterator it = texlist.begin(); it != texlist.end(); it++) {
		Label* lbl = new Label( "", it->second, labelwidth, currpos+100+it->second/2, 0, 0, 0, 0, it->first );
		lbl->color(0,0,0,1);
		cont->add( lbl );
		currpos += it->second+100;
	}

	win->add( cont );
	win->update();
	win->run();
}

