#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <tisch.h>

#include <Region.h>
#include <Gesture.h>
#include <BlobCount.h>
#include <BlobID.h>
#include <BlobPos.h>
#include <Motion.h>

#include <Socket.h>

TCPSocket gestured( INADDR_ANY, 0 );
Region region;

#include <X11/Xlib.h>
//#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/XInput.h>
//#include <X11/extensions/XInput2.h>

std::map<int,Vector> pointers;

Display* display;
int screen, width, height;

typedef struct {
	XDevice dev;
	int axes[2];
	int scroll;
} cursor;

// cursor control data
cursor cur = {
	{ -1, 0, 0 }, // -1 must be replaced by actual master pointer XDevice ID
	{ 700, 520 },
	0
};

// proper signoff on exit
void cleanup( int signal ) {
	std::cout << "Cleaning up.. goodbye." << std::endl;
	gestured << "bye 0" << std::endl;
	gestured.close();
	exit(0);
}


void action( Gesture* gesture ) {

	//std::cout << gesture->name() << std::endl;

	if ( gesture->name() == "remove" ) {
		FeatureBase* f = (*gesture)[0];
		BlobID* m = dynamic_cast<BlobID*>(f);
		pointers.erase( m->result() );
		XTestFakeDeviceButtonEvent( display, &(cur.dev), 1, False, cur.axes, 2, CurrentTime );
	}

	if ( gesture->name() == "move" ) {
		FeatureBase* f = (*gesture)[0];
		BlobID* m = dynamic_cast<BlobID*>(f);

		f = (*gesture)[1];
		BlobPos* p = dynamic_cast<BlobPos*>(f);

		pointers[m->result()] = p->result();

		Vector pos = pointers.begin()->second;
		//int oldx = cur.axes[0];
		int oldy = cur.axes[1];

		int newx = pos.x;
		int newy = height-pos.y;

		if (cur.scroll) {
			if (abs(oldy - newy) < 5) return;
			cur.axes[0] = newx;
			cur.axes[1] = newy;
			int button = 4;
			if (oldy > cur.axes[1]) button = 5;
			XTestFakeDeviceButtonEvent( display, &(cur.dev), button,  True, cur.axes, 2, CurrentTime );
			XTestFakeDeviceButtonEvent( display, &(cur.dev), button, False, cur.axes, 2, CurrentTime );
		}

		cur.axes[0] = newx;
		cur.axes[1] = newy;

		if (!cur.scroll) XTestFakeDeviceMotionEvent( display, &(cur.dev), False, 0, cur.axes, 2, CurrentTime );
	}

	if ( gesture->name() == "tap" ) {

		FeatureBase* f = (*gesture)[0];
		BlobID* m = dynamic_cast<BlobID*>(f);

		f = (*gesture)[1];
		BlobPos* p = dynamic_cast<BlobPos*>(f);

		pointers[m->result()] = p->result();

		Vector pos = pointers.begin()->second;
		cur.axes[0] = pos.x;
		cur.axes[1] = height-pos.y;

		if (pointers.size() == 2)
		{
			Vector pos2 = (++(pointers.begin()))->second;
			if(pos2.x <= pos.x) //left click
				XTestFakeDeviceButtonEvent( display, &(cur.dev), 1,  True, cur.axes, 2, CurrentTime );
			else //right click
				XTestFakeDeviceButtonEvent( display, &(cur.dev), 3,  True, cur.axes, 2, CurrentTime );
		}

		if (pointers.size() == 3) //middle mouse button
			XTestFakeDeviceButtonEvent( display, &(cur.dev), 2,  True, cur.axes, 2, CurrentTime );
	}

	if ( gesture->name() == "release" ) {
		// std::cout << "release" << std::endl;
		cur.scroll = 0;
		XTestFakeDeviceButtonEvent( display, &(cur.dev), 1, False, cur.axes, 2, CurrentTime );
		XTestFakeDeviceButtonEvent( display, &(cur.dev), 2, False, cur.axes, 2, CurrentTime );
		XTestFakeDeviceButtonEvent( display, &(cur.dev), 3, False, cur.axes, 2, CurrentTime );
	}

	if ( gesture->name() == "scroll" ) {
		// std::cout << "release" << std::endl;
		cur.scroll = 1;
	}

	XFlush(display);
}


int process() {

	std::string type;
	int tmp;

	gestured >> type >> tmp;

	if (!gestured || !tmp) {
		gestured.flush();
		return 0;
	}

	if (type.compare("gesture") == 0) {
		Gesture gst; gestured >> gst;
		if (!gestured) { gestured.flush(); return 0; }
		action( &gst );
		return 0;
	}

	return 1;
}


int main( int argc, char* argv[] ) {

	std::cout << "moused - TISCH X11/MPX mouse controller 1.0 beta1" << std::endl;
	std::cout << "(c) 2009 by Florian Echtler <echtler@in.tum.de>" << std::endl;

	if (argc != 2) {
		std::cout << "usage: moused XID" << std::endl;
		std::cout << "XID: XDevice ID of master pointer (see \"xinput list\")" << std::endl;
		exit(1);
	}

	// get XDevice ID
	cur.dev.device_id = atoi(argv[1]);

	// connect to gestured
	gestured.target( INADDR_LOOPBACK, TISCH_PORT_EVENT );

	// open X display
  if ((display = XOpenDisplay(NULL)) == NULL) {
    perror("Can't connect to server");
    exit(1);
  }
  screen = DefaultScreen( display);
	width  = DisplayWidth(  display, screen );
	height = DisplayHeight( display, screen );

	// setup region
	std::cout << "Screen dimensions: " << width << " x " << height << std::endl;

	//region.flags( (1<<INPUT_TYPE_COUNT)-1 );
	region.push_back( Vector(     0,      0 ) );
	region.push_back( Vector( width,      0 ) );
	region.push_back( Vector( width, height ) );
	region.push_back( Vector(     0, height ) );

	// setup gestures
	Gesture move( "move" );
	move.push_back( new BlobID(1<<INPUT_TYPE_FINGER) );
	move.push_back( new BlobPos(1<<INPUT_TYPE_FINGER) );
	region.gestures.push_back( move );

	Gesture tap( "tap", GESTURE_FLAGS_ONESHOT );
	tap.push_back( new BlobID(1<<INPUT_TYPE_FINGER) );
	tap.push_back( new BlobPos(1<<INPUT_TYPE_FINGER) );

	region.gestures.push_back( tap );

	std::vector<int> b2;
	b2.push_back( 0 );
	b2.push_back( 0 );
	BlobCount* bcnt2 = new BlobCount(1<<INPUT_TYPE_FINGER);
	bcnt2->bounds( b2 );

	Gesture release( "release", GESTURE_FLAGS_ONESHOT );
	release.push_back( bcnt2 );

	region.gestures.push_back( release );

	Gesture scroll( "scroll", GESTURE_FLAGS_ONESHOT );
	BlobCount* bcnt3 = new BlobCount(1<<INPUT_TYPE_FINGER);
	b2.clear(); b2.push_back( 4 ); b2.push_back( 4 );
	bcnt3->bounds( b2 );
	scroll.push_back( bcnt3 );
	region.gestures.push_back( scroll );

	Gesture remove( "remove", GESTURE_FLAGS_ONESHOT );
	BlobID* bid = new BlobID( 1 << INPUT_TYPE_FINGER );
	b2.clear(); b2.push_back( -1 ); bid->bounds( b2 );
	remove.push_back( bid );
	region.gestures.push_back( remove );

	// transmit
	gestured << "region 1234 " << region << std::endl;

	signal( SIGINT, cleanup );

	// process
	while (process() == 0) { }

	// cleanup
	cleanup(0);
}

