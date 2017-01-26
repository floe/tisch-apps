/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006 - 2009 by Florian Echtler, TUM <echtler@in.tum.de>  *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <tisch.h>
#include "Window.h"
#include "Tile.h"
#include <Scanner.h>
#include <BasicBlob.h>
#include <PicoPNG.h>
#include <BlobCount.h>
#include <BlobPos.h>
#include <BlobDim.h>
#include <BlobID.h>
#include <Rotation.h>
#include <Scale.h>
#include <Motion.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#define snprintf _snprintf
#define srandom srand
#define random rand
#endif

#define SQUARE 9


int slotsize = 40;
int tilesize = slotsize-5;
int fieldwidth = (9*slotsize)/2;

unsigned char statebuf[82] = 
	"00H0F0G00"
	"F0000C0I0"
	"000H00B00"
	"00AF0G0C0"
	"0IF000HG0"
	"0E0I0HD00"
	"00E00A000"
	"0G0B0000D"
	"00D0G0I00";


unsigned char* gamestate[9] = { statebuf, statebuf+9, statebuf+18, statebuf+27, statebuf+36, statebuf+45, statebuf+54, statebuf+63, statebuf+72 };

double grid[9][9][2];


class SudokuWindow: public Window {

	public:

		SudokuWindow( int width, int height, const char* name, int use_mouse ):
			Window(width,height,name,use_mouse)
		{
			for (int i = 0; i < 9; i++) for (int j = 0; j < 9; j++) {
				grid[i][j][0] = -fieldwidth+slotsize*(i+0.5);
				grid[i][j][1] = -fieldwidth+slotsize*(j+0.5);
			}

			/*
			BlobID* gone = new BlobID(1<<INPUT_TYPE_HAND);
			gone->bounds().push_back(-1);
			Gesture vanish( "vanish" );
			vanish.push_back( gone );
			region.gestures.push_back(vanish);

			Gesture move( "move" );
			move.push_back( new BlobID(1<<INPUT_TYPE_HAND) );
			move.push_back( new BlobPos(1<<INPUT_TYPE_HAND) );
			region.gestures.push_back( move );
			*/

			//region.flags(  );
			mycolor[0] = 0.0;
			mycolor[1] = 0.0;
			mycolor[2] = 0.0;
		}

		/*
		void action( Gesture* gesture ) {

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
			}

		}
		*/

		virtual void draw() {

			Container::draw();

			enter();

			glDisable(GL_TEXTURE_2D);
			glColor4f( 0.0, 0.0, 1.0, 1.0 );

			GLvector vertices[4];

			glTranslatef(0,0,0.1);
			for (int i = 0; i < 10; i++) {
				if ((i % 3) == 0) glLineWidth(5.0);
				vertices[0] = GLvector( -fieldwidth+slotsize*i, -fieldwidth );
				vertices[1] = GLvector( -fieldwidth+slotsize*i,  fieldwidth );
				vertices[2] = GLvector(  fieldwidth, -fieldwidth+slotsize*i );
				vertices[3] = GLvector( -fieldwidth, -fieldwidth+slotsize*i );
				glutPaintArrays( 4, vertices, (GLvector*)0, (GLvector*)0, GL_LINES );
				glLineWidth(1.0);
			}

			glPointSize(5.0);
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


#ifdef TISCH_BLUETOOTH

class JoinArea: public Button {

	public:

		JoinArea( int w, int h, int x, int y ): Button(w,h,x,y,0,0) {

			normal = new RGBATexture("join.png");
			mytex = normal;
			region.flags( 1<<INPUT_TYPE_SHADOW );

			std::vector<int> bounds; bounds.push_back( -1 );
			BlobID* gone = new BlobID();
			gone->bounds( bounds );
			Gesture vanish( "vanish" );
			vanish.push_back( gone );
			region.gestures.push_back(vanish);

			std::vector<Dimensions> bdim;
			Dimensions lower = {   0,  0,  0,  0, 1000 }; bdim.push_back( lower );
			Dimensions upper = { 100,100,100,100,10000 }; bdim.push_back( upper );
			BlobDim* dim = new BlobDim();
			dim->bounds( bdim );

			Gesture mopho( "mopho", GESTURE_FLAGS_ONESHOT );
			mopho.push_back( dim );
			mopho.push_back( new BlobPos() );
			mopho.push_back( new BlobID() );

			region.gestures.push_back( mopho );

			scan = new Scanner( "/org/bluez/hci0", -65 );
			scan->start();
			cycle = 0;
		}

		~JoinArea() { delete mytex; }

		void draw() {
			enter();
			paint();
			GLUquadric* quad = gluNewQuadric();
			if (cycle++ > 100) cycle = 20;
			glColor4f( (double)(cycle)/100.0, 0.0, 0.0, 1.0 );
			for (std::map<int,Vector>::iterator cand = candidates.begin(); cand != candidates.end(); cand++) {
				glPushMatrix();
				glTranslatef(cand->second.x,cand->second.y,0);
				gluDisk( quad, 0, 30, 16, 1 );
				glPopMatrix();
			}
			leave();
		}

		void action( Gesture* gesture ) {
			
			if (gesture->name() == "vanish") {
				BlobID* i = dynamic_cast<BlobID*>((*gesture)[0].get());
				candidates.erase( i->result() );
			}

			if (gesture->name() == "mopho") {

				//std::cout << "detected phone candidate" << std::endl;
				BlobPos* p = dynamic_cast<BlobPos*>((*gesture)[1].get());
				BlobID* i = dynamic_cast<BlobID*>((*gesture)[2].get());
				Vector tmp = p->result();
				transform( tmp, 1 );
				candidates[i->result()] = tmp;

				DevMap* btdevs = scan->getDevices();

				for (DevMap::iterator it = btdevs->begin(); it != btdevs->end(); it++) {

					int found = 0;
					for (std::vector<std::string>::iterator old = processed.begin(); old != processed.end(); old++)
						if (old->compare(it->first) == 0) found = 1;
					if (found) continue;

					char buffer[1024];
					snprintf( buffer, sizeof(buffer), "bluetooth-sendto --device=%s sudoku.jar & disown", it->first.c_str() );
					system( buffer );
					processed.push_back( it->first );
				}

				delete btdevs;
			} //else Button::action( gesture );
		}

	protected:

		Scanner* scan;
		RGBATexture* normal;
		RGBATexture* warn;

		std::vector<std::string> processed;
		std::map<int,Vector> candidates;
		int cycle;

		//void tap( Vector pos, int id ) { std::cout << id << "@" << pos << std::endl; }
};

#endif



class SudokuTile: public Tile {

	public:

	SudokuTile( RGBATexture* tex, int w, int h, int x, int y ): Tile(w,h,x,y,0,tex,TISCH_TILE_MOVE|TISCH_TILE_ROTATE/*|TISCH_TILE_SLIDE|TISCH_TILE_BOUNCE*/) {
		snap = 1; lock = 0; tx = ty = 0; 
	}

	void action( Gesture* gesture ) {

		if (lock) return;

		double oldx = x; 
		double oldy = y;

		Tile::action(gesture);

		if (snap) {
			tx += x-oldx; ty += y-oldy;
			x = oldx; y = oldy;
			if ((fabs(tx)<5) && (fabs(ty)<5)) return;
			snap = 0; x += tx; y += ty;
		}
	}

	/*void draw() {
		double tangle = angle; angle = 0;
		enter();
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glRotatef(360*tangle/(2*M_PI),0,0,1);
		glMatrixMode(GL_MODELVIEW);
		paint();
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		leave();
		angle = tangle;
	}*/

	void move( double _x, double _y ) {
		x = _x; y = _y;
	}

	int snap,lock;

	private:

		double tx,ty;

};



RGBATexture* texids[9][2];


// what's missing in order of importance:
// - snap-to-grid
// - rotation of tile content
// - server comm


int main( int argc, char* argv[] ) {

	std::cout << "sudoku - TISCH widget layer demo 1.0 beta2" << std::endl;
	std::cout << "(c) 2008,09 by Florian Echtler <echtler@in.tum.de>" << std::endl;

	int mouse = 0;
	int mopho = 0;

	for ( int opt = 1; opt < argc; opt++ ) {
		if (std::string(argv[opt]) == "-m") mouse = 1;
		if (std::string(argv[opt]) == "-p") mopho = 1;
	}

	SudokuWindow* win = new SudokuWindow( 640, 480, "Sudoku", mouse );

	// load the textures
	for (int i = 0; i < 9; i++) {
		char file[1024];
		snprintf( file, sizeof(file), "single/color%d.png", i+1 ); texids[i][0] = new RGBATexture( file );
		snprintf( file, sizeof(file), "single/fixed%d.png", i+1 ); texids[i][1] = new RGBATexture( file );
	}

	#ifdef TISCH_BLUETOOTH
	if (mopho) {
		JoinArea* join = new JoinArea(125,300,-250,0);
		win->add(join);
	}
	#endif

	SudokuTile* tiles[9][9];

	srandom(45890);

	for (int i = 0; i < SQUARE; i++) 
		for (int j = 0; j < SQUARE; j++) {
			int fac = (mopho ? 1 : (j%2)*2-1 );
			int y = (int)(((double)random()/(double)RAND_MAX)*400-200);
			int x = (int)((((double)random()/(double)RAND_MAX)*100+200)) * fac;
			tiles[i][j] = new SudokuTile( texids[i][0], tilesize, tilesize, x, y );
			// -fieldwidth+slotsize*(i+0.5), -fieldwidth+slotsize*(j+0.5) ); 
			win->add(tiles[i][j]);
		}

	int cnt[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < SQUARE; i++) 
		for (int j = 0; j < SQUARE; j++) {
			unsigned char cell = gamestate[i][j];
			if ((cell >= '1') && (cell <= '9')) {
				int num = cell-'1';
				tiles[num][cnt[num]++]->move( grid[i][j][0], grid[i][j][1] );
			}
			if ((cell >= 'A') && (cell <= 'I')) {
				int num = cell-'A';
				SudokuTile* tile = tiles[num][cnt[num]++];
				tile->move( grid[i][j][0], grid[i][j][1] );
				tile->lock = 1;
				tile->texture(texids[num][1]);
			}
		}
	
	win->update();
	win->run();
}

