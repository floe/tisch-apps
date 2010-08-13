#ifndef _MIDIOUTPUT_H_
#define _MIDIOUTPUT_H_

#include <alsa/asoundlib.h>
#include <stdint.h>


#define MIDI_NOTE_ON  0x09
#define MIDI_NOTE_OFF 0x08
#define MIDI_PGM_CHNG 0x0C


class MIDICommand {

	public:

		MIDICommand( uint8_t cmd, uint8_t channel, uint8_t param1, uint8_t param2 = 0 );

		snd_seq_event_t ev;
};


class MIDIOutput {

	public:
	
		MIDIOutput( const char* mididev = "128:0" );
		~MIDIOutput();

		void send( MIDICommand cmd );

	protected:

		snd_seq_t* handle;
		int port;

};

#endif // _MIDIOUTPUT_H_

