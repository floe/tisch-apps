/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "Window.h"
#include "Tile.h"
#include <Container.h>
#include <BasicBlob.h>
#include <PicoPNG.h>
#include <BlobParent.h>
#include <BlobPos.h>
#include <BlobGroup.h>
#include <BlobID.h>
#include <stdlib.h>

void scanline( RGBImage* target, int x1, int x2, int y, Color color ) {

	if (x1 < 0) x1 = 0;
	if (x2 < 0) x2 = 0;
	if (y  < 0) return;

	if (x1 >= target->getWidth()) x1 = target->getWidth()-1;
	if (x2 >= target->getWidth()) x2 = target->getWidth()-1;
	if (y >= target->getHeight()) return;

	if (x1 > x2) { int tmp = x1; x1 = x2; x2 = tmp; }

	for (int i = x1; i <= x2; i++) target->setPixel( i, y, color.x, color.y, color.z );
}


void circle( RGBImage* target, Vector pos, Color color ) {

	int cx = (int)(pos.x);
	int cy = target->getHeight() - (int)(pos.y);
	int radius = (int)(5);

  int x = 0;
  int y = radius;
  int d = 3 - 2*radius;
  //unsigned char tmp;

  // ye olde Bresenham circle algorithm
  while (x <= y) {

		scanline( target, cx+x, cx-x, cy+y, color );
		scanline( target, cx+x, cx-x, cy-y, color );
		scanline( target, cx+y, cx-y, cy+x, color );
		scanline( target, cx+y, cx-y, cy-x, color );

    if (d < 0) {
      d = d + 4*x + 6;
    } else {
      d = d + 4*(x-y) + 10;
      y = y - 1;
    }
    x = x + 1;
  }
}

RGBImage background(1024,1024);
RGBATexture* tex;


// eugh.. global color map
std::map<int,Color> colors;

class ColorMenu: public Widget {
	public:
		ColorMenu(int w, int h, int x, int y): Widget(w,h,x,y) {
			Gesture tap( "tap", GESTURE_FLAGS_ONESHOT );
			tap.push_back( new BlobParent( 1<<INPUT_TYPE_FINGER ) );
			region.gestures.push_back( tap );
			region.flags( 1 << INPUT_TYPE_FINGER );
		}
		~ColorMenu() { }

		void draw() {
			enter();
			glColor4f(0,1,0,1);
			paint();
			leave();
		}

		void action( Gesture* gesture ) {

			if (gesture->name() == "tap" ) {
				
				FeatureBase* f = (*gesture)[0];
				BlobParent* p = dynamic_cast<BlobParent*>(f);
				colors[p->result()] = Color(0,255,0);

				Container* par = dynamic_cast<Container*>(parent);
				par->remove( this );
				par->update();

				delete this;
			}
		}
};



class Whiteboard: public Window {

	public:

		Whiteboard( int width, int height, const char* name, int use_mouse ):
			Window(width,height,name,use_mouse)
		{
			Gesture gmenu( "menu", GESTURE_FLAGS_STICKY|GESTURE_FLAGS_ONESHOT );
			std::vector<Vector> mbounds; mbounds.push_back( Vector(2,20,0) );
			BlobGroup* group = new BlobGroup(1<<INPUT_TYPE_FINGER);
			group->bounds( mbounds );
			gmenu.push_back( group );
			region.gestures.push_back( gmenu );

			Gesture move( "move" );
			move.push_back( new BlobID(1<<INPUT_TYPE_SHADOW) );
			move.push_back( new BlobPos(1<<INPUT_TYPE_SHADOW) );
			region.gestures.push_back( move );

			std::vector<int> bounds; bounds.push_back( -1 );
			BlobID* gone = new BlobID(1<<INPUT_TYPE_SHADOW);
			gone->bounds( bounds );
			Gesture vanish( "vanish" );
			vanish.push_back( gone );
			region.gestures.push_back(vanish);

			Gesture fmove( "fmove" );
			fmove.push_back( new BlobParent(1<<INPUT_TYPE_FINGER) );
			fmove.push_back( new BlobPos(1<<INPUT_TYPE_FINGER) );
			region.gestures.push_back( fmove );
		}


		void action( Gesture* gesture ) {

			if (gesture->name() == "menu") {
				BlobGroup* g = dynamic_cast<BlobGroup*>((*gesture)[0].get());
				Vector tmp = g->result();
				transform( tmp, 1 );
				add( new ColorMenu(20,20,tmp.x,tmp.y) );
				update();
			}

			if (gesture->name() == "fmove" ) {
				BlobParent* par = dynamic_cast<BlobParent*>((*gesture)[0].get());
				BlobPos* p = dynamic_cast<BlobPos*>((*gesture)[1].get());
				if (!p || !par) return;
				Vector tmp = p->result();
				transform( tmp, 1 );
				tmp.x = 1024*(tmp.x/w) + 512;
				tmp.y = 1024*(tmp.y/h) + 512;
				circle(&background,tmp,colors[par->result()]);
				//std::cout << "blob: " << i->result() << " parent: " << par->result() << std::endl;
			}

			if (gesture->name() == "move" ) {
				BlobID*  i = dynamic_cast<BlobID* >((*gesture)[0].get());
				BlobPos* p = dynamic_cast<BlobPos*>((*gesture)[1].get());
				if (!p || !i) return;
				Vector tmp = p->result();
				transform( tmp, 1 );
				shadows[i->result()] = tmp;
				//std::cout << "got blob: " << m->result() << std::endl;
			}

			if (gesture->name() == "vanish") {
				FeatureBase* f = (*gesture)[0];
				BlobID* i = dynamic_cast<BlobID*>(f);
				if (!i) return;
				//std::cout << "blob vanished: " << i->result() << std::endl;
				shadows.erase(i->result());
				colors.erase(i->result());
			}

		}

		virtual void draw() {

			tex->load( &background );
			Container::draw();

			enter();

			GLvector vertices[3];

			glColor4f( 0.0, 0.0, 0.0, 1.0 );
			for (std::map<int,Vector>::iterator shadow = shadows.begin(); shadow != shadows.end(); shadow++) {
				vertices[0] = GLvector( shadow->second.x,    shadow->second.y    );
				vertices[1] = GLvector( shadow->second.x-10, shadow->second.y    );
				vertices[2] = GLvector( shadow->second.x,    shadow->second.y+10 );
				glutPaintArrays( 3, vertices, (GLvector*)0, (GLvector*)0, GL_TRIANGLES );
			}

			glEnable(GL_TEXTURE_2D);

			leave();
		}

	private:

		std::map<int,Vector> shadows;
};



int main( int argc, char* argv[] ) {

	std::cout << "vwb - TISCH widget layer demo 1.0 beta2" << std::endl;
	std::cout << "(c) 2009 by Florian Echtler <echtler@in.tum.de>" << std::endl;

	int mouse = 0;

	for ( int opt = 1; opt < argc; opt++ ) {
		if (std::string(argv[opt]) == "-m") mouse = 1;
	}

	Whiteboard* win = new Whiteboard( 640, 480, "Whiteboard", mouse );

	background.clear( 255 );
	tex = new RGBATexture(1024,1024);
	tex->load( &background );
	win->texture( tex );

	win->update();
	win->run();
}

