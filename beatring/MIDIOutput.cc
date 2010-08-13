#include <stdexcept>
#include <iostream>

#include "MIDIOutput.h"


MIDICommand::MIDICommand( uint8_t cmd, uint8_t channel, uint8_t param1, uint8_t param2 ) {

	snd_seq_ev_clear( &ev );
	snd_seq_ev_set_subs( &ev );
	snd_seq_ev_set_direct( &ev );

	switch (cmd) {
		case MIDI_NOTE_ON:  snd_seq_ev_set_noteon   ( &ev, channel, param1, param2 ); break;
		case MIDI_NOTE_OFF: snd_seq_ev_set_noteoff  ( &ev, channel, param1, param2 ); break;
		case MIDI_PGM_CHNG: snd_seq_ev_set_pgmchange( &ev, channel, param1         ); break;
	}
}


MIDIOutput::MIDIOutput( const char* mididev ) {

	/* midi key offsets for whole notes
	int wkeynums[7] = { 0, 2, 4, 5, 7, 9, 11 };

	// midi key offsets for half notes
	int bkeynums[5] = { 1, 3, 6, 8, 10 };

	white_key( xo, yo, BASE_NOTE+(12*i)+wkeynums[j] );

	black_key( xo, yo, BASE_NOTE+(12*i)+bkeynums[j] );*/

	// open the midi device or throw
	/*if (int err = snd_rawmidi_open( NULL, &midi, mididev, 0 ))
		throw std::runtime_error( std::string( "snd_rawmidi_open( " ) + std::string(mididev) + " ): " + std::string(strerror(-err)) );*/

	int err = 0;
	if ((err = snd_seq_open( &handle, "default", SND_SEQ_OPEN_OUTPUT, 0 )) < 0)
		throw std::runtime_error( std::string( "snd_seq_open(): " ) + std::string(snd_strerror(err)) );

	snd_seq_set_client_name( handle, "libTISCH MIDI Client" );

	if ((port = snd_seq_create_simple_port(
		handle, 
		"Default Port",
		SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
		SND_SEQ_PORT_TYPE_MIDI_GENERIC
	)) < 0)
		throw std::runtime_error( std::string( "snd_seq_create_simple_port(): " ) + std::string(snd_strerror(err)) );

	snd_seq_addr addr;
	snd_seq_parse_address( handle, &addr, mididev );

	if ((err = snd_seq_connect_to( handle, port, addr.client, addr.port )) < 0)
		throw std::runtime_error( std::string( "snd_seq_connect_to( " ) + std::string(mididev) + " ): " + std::string(snd_strerror(err)) );
}

MIDIOutput::~MIDIOutput( ) {
	//snd_rawmidi_close( midi );
	snd_seq_close( handle );
}


void MIDIOutput::send( MIDICommand cmd ) {

	// send command
	snd_seq_ev_set_source( &(cmd.ev), port );

	int err = snd_seq_event_output_direct( handle, &(cmd.ev) );
	if (err < 0) std::cerr << "snd_seq_event_output(): " << snd_strerror(err) << std::endl;

	err = snd_seq_drain_output( handle );
	if (err < 0) std::cerr << "snd_seq_drain_output(): " << snd_strerror(err) << std::endl;

	//snd_rawmidi_write( midi, cmd.buffer, cmd.size );
	// flush midi buffers - makes things awfully slow
 	//snd_rawmidi_drain( midi );
}

