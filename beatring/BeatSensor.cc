#include "BeatSensor.h"
#include "BlobID.h"
#include "BlobPos.h"

BeatSensor::BeatSensor( int _channel, int _note, int _program, int _rad, int _x, int _y ):
	Button( 2*_rad, 2*_rad, _x, _y ), active(0),
	cmd1( MIDI_NOTE_ON,  _channel, _note, 127 ),
	cmd2( MIDI_NOTE_OFF, _channel, _note, 127 )
{
	radius = _rad;
	quad = gluNewQuadric();
	region.flags( (1<<INPUT_TYPE_COUNT)-1 );

	Gesture tap("tap",GESTURE_FLAGS_ONESHOT);
	tap.push_back( new BlobID() );
	tap.push_back( new BlobPos() );
	region.gestures.push_back( tap );

	MIDICommand pgm( MIDI_PGM_CHNG, _channel, _program );
	midi->send( pgm );
}

BeatSensor::~BeatSensor() {
	gluDeleteQuadric( quad );
}

void BeatSensor::draw() {
	enter();
	glDisable(GL_TEXTURE_2D);
	if (active) glColor4f( 1.0, 0.0, 0.0, 1.0 );
	       else glColor4f( 1.0, 1.0, 0.0, 1.0 );
	gluDisk( quad, 0, radius, 16, 1 );
	leave();

}

void BeatSensor::tap( Vector pos, int id ) { active = 1; }
void BeatSensor::release() { active = 0; midi->send( cmd2 ); }

void BeatSensor::check() {
	midi->send( cmd2 );
	if (active) midi->send( cmd1 );
}

