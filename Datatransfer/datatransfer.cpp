/*************************************************************************\
*    Datatransfer - libTISCH demo - see http://tisch.sourceforge.net/     *
*     Copyright (c) 2012 by Norbert Wiedermann, <wiederma@in.tum.de>      *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdlib.h>
#include <nanolibc.h>

#include "Window.h"
#include <typeinfo>
#include <iostream>
#include <sstream>
#include "BlobMarker.h"
#include "BlobCount.h"
#include "BlobPos.h"

#include "Socket.h"


using namespace std;

Window* win = 0;



class HandyDropZone: public Tile {
public: HandyDropZone( int _x, int _y, RGBATexture* _tex):
	Tile(100,200, _x, _y, 0, _tex, TISCH_TILE_MOVE|TISCH_TILE_SLIDE|TISCH_TILE_BOUNCE )
	{
		shadow = true;
	}
};

class Handy: public Container {
public:
	Handy(int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0xFF):
	Container( _w, _h, _x, _y, angle, _tex, mode)
	{
	  
		for(vector<Gesture>::iterator iter = region.gestures.begin();
			iter != region.gestures.end(); iter++)
		{
			if( iter->name() == "handy" ) {
				region.gestures.erase(iter);
				break;
			}

		}

		Gesture handy( "handy", GESTURE_FLAGS_DEFAULT|GESTURE_FLAGS_STICKY);

		//
		handy.push_back(new BlobMarker(1<<INPUT_TYPE_OBJECT));
		
		//
		BlobCount* bcnt = new BlobCount(1<<INPUT_TYPE_OBJECT);
		bcnt->bounds().push_back(0);
		bcnt->bounds().push_back(1);
		handy.push_back( bcnt );

		//
		handy.push_back(new BlobPos(1<<INPUT_TYPE_OBJECT));

		//
		region.gestures.push_back( handy );
	}

	void action( Gesture* gesture ) {
		if( gesture->name() == "handy" ) {

			FeatureBase* f = (*gesture)[1];
			BlobCount* p = dynamic_cast<BlobCount*>(f);

			if(p->result() == 1) {
				f = (*gesture)[0];
				BlobMarker* bm = dynamic_cast<BlobMarker*>(f);
				
				if(bm->markerID > 0) {
					RGBATexture* texture = new RGBATexture( TISCH_PREFIX "Box.png" );
					
					f = (*gesture)[2];
					BlobPos* bp = dynamic_cast<BlobPos*>(f);
					Vector blobPos = bp->result();
					transform(blobPos,1);
					blobPos.z = 0;
					blobPos.normalize();
					
					HandyDropZone* hdz = new HandyDropZone(blobPos.x, blobPos.z, texture);
					win->add(hdz);
				}

			} // if(p->result() == 1) {

		} // if( gesture->name() == "handy" ) {
		Container::action(gesture);

	} // void action( Gesture* gesture ) {
};



int main( int argc, char* argv[] ) {
	cout << "Datatransfer - libTISCH demo" << endl;
	cout << "by Norbert Wiedermann (wiederma@in.tum.de) 2012" << endl;

	int width = 640;
	int height = 480;

	int mouse = ((argc > 1) && (std::string("-m") == argv[1]));
	win = new Window( width, height, "Datatransfer", mouse );
	win->texture(0);

	Handy* handy = new Handy( width, height );
	win->add(handy);
	
	

	win->update();
	win->run();
}
