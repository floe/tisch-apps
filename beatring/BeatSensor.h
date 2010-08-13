/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#ifndef _BEATSENSOR_H_
#define _BEATSENSOR_H_

#include "Button.h"
#include "MIDIOutput.h"

extern MIDIOutput* midi;

class TISCH_SHARED BeatSensor: public Button {

	public:

		BeatSensor( int _channel, int _note, int _program, int _rad, int _x = 0, int _y = 0 ); 
		virtual ~BeatSensor(); 

		virtual void draw();

		virtual void tap( Vector pos, int id );
		virtual void release();

		void check();

	protected:

		int active;
		double radius;

		MIDICommand cmd1;
		MIDICommand cmd2;
		GLUquadric* quad;
};

#endif // _BEATSENSOR_H_

