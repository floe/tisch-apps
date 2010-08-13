/*
 * some notes about midi output:
 * - start 'timidity -iA -B2,8 -Os -EFreverb=0' to create a virtual midi sink (128:0 - 128:3)
 * - use MIDI client '128:0'
*/


#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>

#include "tisch.h"
#include "Window.h"
#include "Dial.h"
#include "MIDIOutput.h"
#include "BeatSensor.h"


class Pointer: public Widget {
public:
	Pointer( int w, int h ): Widget( w, h ) { }
	~Pointer() { }
	void action( Gesture* foo ) { }
	void rotate( double _angle ) { angle = _angle; }
	void draw() { 
		enter();
		// compensate latency by turning the pointer back by 15 degrees
		glRotatef( -15, 0, 0, 1 );
		glColor4f(1.0,0.0,0.0,1.0);
		glLineWidth(3.0);
		glBegin(GL_LINES);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(400.0,0.0,0.0);
		glEnd();
		leave();
	}
};

#define TIMER 15 // ms

Pointer* point;

Window* win;
MIDIOutput* midi;
Dial* dial;

BeatSensor* beats[4][8];

int angle = 0;


void mytimer( int val ) {
	angle++; if (angle == 80) angle = 0;
	point->rotate( M_PI*(angle/40.0) );
	if (angle % 10 == 0) 
		for (int c = 0; c < 4; c++) 
			beats[c][angle/10]->check();
	double timeout = TIMER+(0.5-dial->get())*10;
	glutTimerFunc( timeout, mytimer, 0 );
}


int xres = 800;
int yres = 600;

int main(int argc, const char* argv[]) {

	const char* mididev = "128:0";
	//if (argc >= 2) mididev = argv[1];

	std::cout << "beatring - TISCH widget layer demo 1.0 beta1" << std::endl;
	std::cout << "(c) 2009 by Florian Echtler <echtler@in.tum.de>" << std::endl;

	midi = new MIDIOutput( mididev );
	win = new Window( xres, yres, "beatring", (argc > 1) );
	win->texture(0);
	win->color(0.0,0.0,0.0,1.0);

	int instruments[] = { 36, 40, 46, 49 };
	// 36: Bass Drum
	// 40: Electric Snare
	// 46: Open Hi-Hat
	// 49: Crash Cymbal

	for (int c = 0; c < 4; c++) {
		Vector p((c+1)*75,0,0);
		for (int s = 0; s < 8; s++) {
			// channel 10 is GM percussion channel, note = instrument
			// all other channels: program = instrument, note = tone height
			// BeatSensor( channel_zero_based, note, program, radius, x, y )
			beats[c][s] = new BeatSensor( 9, instruments[c], 0, 35, p.x, p.y ); 
			win->add( beats[c][s] );
			p.rotate( -M_PI/4 );
		}
	}

	point = new Pointer( 25, 25 );
	win->add( point );

	dial = new Dial( 50 );
	win->add( dial );

	glutTimerFunc( TIMER, mytimer, 0 );

	win->update();
	win->run();

	delete win;
	delete midi;

	return 0;
}

