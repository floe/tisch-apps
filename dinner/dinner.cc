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

RGBATexture* tex;

class DinnerTracker: public Window {

	public:

		DinnerTracker( int width, int height, const char* name, int use_mouse ):
			Window(width,height,name,use_mouse)
		{
			Gesture move( "smove" );
			move.push_back( new BlobID(1<<INPUT_TYPE_HAND) );
			move.push_back( new BlobPos(1<<INPUT_TYPE_HAND) );
			region.gestures.push_back( move );

			BlobID* gone = new BlobID(1<<INPUT_TYPE_HAND);
			gone->bounds().push_back( -1 );
			Gesture vanish( "vanish" );
			vanish.push_back( gone );
			region.gestures.push_back(vanish);

			color(0,0,0,1);
			texture(NULL);
			region.flags(0xFFFFFFFF);
			shadows.clear();
		}


		void action( Gesture* gesture ) {

			if (gesture->name() == "smove" ) {
				BlobID*  i = dynamic_cast<BlobID* >((FeatureBase*)((*gesture)[0]));
				BlobPos* p = dynamic_cast<BlobPos*>((FeatureBase*)((*gesture)[1]));
				if (!p || !i) return;
				//std::cout << "got blob " << i->result() << " " << i->has_result << std::endl;
				Vector tmp = p->result();
				transform( tmp, 1 );
				shadows[i->result()] = tmp;
			}

			if (gesture->name() == "vanish") {
				FeatureBase* f = (*gesture)[0];
				BlobID* i = dynamic_cast<BlobID*>(f);
				if (!i) return;
				shadows.erase(i->result());
			}

		}

		virtual void draw() {

			Container::draw();

			enter();

			for (std::map<int,Vector>::iterator shadow = shadows.begin(); shadow != shadows.end(); shadow++) {
				//std::cout << "drawing blob: " << shadow->first << " " << shadow->second << std::endl;
				glPushMatrix( );
				glTranslatef( shadow->second[0]+(tex->width()/2), shadow->second[1], 0 );
				glScalef(0.5,0.5,1);
				paint_tex(tex);
				glPopMatrix( );
			}

			leave();
		}

	private:

		std::map<int,Vector> shadows;
};



int main( int argc, char* argv[] ) {

	std::cout << "dinner - TISCH widget layer demo 1.0 beta2" << std::endl;
	std::cout << "(c) 2009 by Florian Echtler <echtler@in.tum.de>" << std::endl;

	int mouse = 0;

	for ( int opt = 1; opt < argc; opt++ ) {
		if (std::string(argv[opt]) == "-m") mouse = 1;
	}

	DinnerTracker* win = new DinnerTracker( 800, 600, "dinner", mouse );
	tex = new RGBATexture("nutrition.png");

	win->update();
	win->run();
}

