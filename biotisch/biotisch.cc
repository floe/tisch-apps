/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <Window.h>
#include <Widget.h>
#include <Thread.h>
#include <Label.h>
#include <Textbox.h>
#include <Container.h>
#include <Rotation.h>
#include <Scale.h>
#include <Motion.h>
#include <stdlib.h>
#include <fstream>

#ifdef _WIN32
	#define snprintf _snprintf
	#define srandom srand
	#define random rand
#endif


// plasmid viewer
Container* plasview;

// calculator

Container* calc;
Textbox* txtbox;
Label* result;

struct CalcButton: public Button {

	public:

		CalcButton( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0 ):
			Button( _w,_h,_x,_y,angle,_tex) { }

		void tap( Vector pos, int id ) {
			std::string cmd = txtbox->get();
			// TODO: magic equation parsing/evaluation here
			result->set( cmd );
			txtbox->set( "" );
		}
	
};

// experiment log

std::vector<std::string> logtext;
int logpos = 0;
Container* logview;
Label* logitem1;
Label* logitem2;
Label* logitem3;

struct NextLogButton: public Button {

	public:

		NextLogButton( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0 ):
			Button( _w,_h,_x,_y,angle,_tex) { }

		void tap( Vector pos, int id ) {
			if (logpos >= logtext.size()-1) return;
			logpos++;
			logitem1->set( std::string("   ") + logtext[logpos-1] );
			logitem2->set( std::string("-> ") + logtext[logpos]   );
			if (logpos != logtext.size()-1)
				logitem3->set( std::string("   ") + logtext[logpos+1] );
			else
				logitem3->set( "" );
		}
};

// bottle sensors

struct BottleSensor: public Button {

	public:

		BottleSensor( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0 ):
			Button( _w,_h,_x,_y,angle,_tex) 
		{
			getDefaultTex( 0, "Dial.png" );
			// react not to touches, but to object shadows
			region.flags( 1<<INPUT_TYPE_SHADOW );
			//mode = 0;
			shadow = 0;
			release();
		}

		void tap( Vector pos, int id ) {
			color( 0, 1, 0, 1 );
		}

		void release() {
			active = 1;
			color( 1, 0, 0, 1 );
		}

		//int mode;
};

void split( const std::string& s, char c, std::vector<std::string>& v ) {
	std::string::size_type i = 0;
	std::string::size_type j = s.find(c);
	while (j != std::string::npos) {
		v.push_back(s.substr(i, j-i));
		i = ++j;
		j = s.find(c, j);
		if (j == std::string::npos)
			v.push_back(s.substr(i, s.length()));
	}
}



// main program
int main( int argc, char* argv[] ) {

	std::cout << "biotisch - TISCH molecular biology interface 1.0 alpha1" << std::endl;
	std::cout << "(c) 2009 by Florian Echtler <echtler@in.tum.de>" << std::endl;

	// load log
	std::ifstream logfile( "log.txt" );

	while (logfile) {

		std::string line;
		std::vector<std::string> tokens;
		std::getline( logfile, line );
		split( line, ',', tokens );

		if (logfile) 
			logtext.push_back( tokens[0]+" "+tokens[2] );
	}

	Window* win = new Window( 1024, 768, "biotisch", ((argc > 1) ) );

	// calculator: http://www.lochan.org/2005/keith-cl/concalc.html
	calc = new Container( 300, 100, -300, -200, 0, 0, 0xFF );

	calc->add( txtbox = new Textbox( 200, 35, 140, -50,-25 ) );
	calc->add( result = new Label( "Result: 0", 300, 25, 0, 25 ) );
	calc->add( new CalcButton( 100, 35, 100, -25 ) );
	calc->add( new Label( "Calculate", 90, 35, 100, -25 ) );

	win->add( calc );

	// plasmid viewer: http://wishart.biology.ualberta.ca/PlasMapper/jsp/librarySeq.jsp?id=Invitrogen33
	plasview = new Container( 300, 270,  0, 0,  0, 0, 0xFF );
	RGBATexture* ptex = new RGBATexture( "plasmid.png" );
	plasview->texture( ptex ); 

	win->add(plasview);

	// log viewer
	logview = new Container( 300, 180, 300, -200, 0, 0, 0xFF );
	
	logview->add( new Label( "Experiment Log", 300, 20, 0, 50 ) );
	logview->add( logitem1 = new Label(              "   "                     , 300, 20, 0,  25 ) );
	logview->add( logitem2 = new Label( (std::string("-> ")+logtext[0]).c_str(), 300, 20, 0,   0 ) );
	logview->add( logitem3 = new Label( (std::string("   ")+logtext[1]).c_str(), 300, 20, 0, -25 ) );
	logview->add( new NextLogButton( 100, 30, 0, -70 ) );
	logview->add( new Label( "Next Step", 100, 30, 0, -70 ) );
	logitem1->color( 0.7,0.7,0.7,1.0);
	logitem3->color( 0.7,0.7,0.7,1.0);

	win->add( logview );

	// bottle indicators
	win->add( new BottleSensor( 200, 200, -350, 250 ) );
	win->add( new Label( "H2O",  60,  30, -350, 250 ) );
	win->add( new BottleSensor( 200, 200, -150, 250 ) );
	win->add( new Label( "Buffer", 100,  30, -150, 250 ) );


	win->run();
}

