/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*  Copyright (c) 2006,07,08 by Florian Echtler, TUM <echtler@in.tum.de>   *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <Window.h>
#include <Widget.h>
#include <Thread.h>
#include <Rotation.h>
#include <Scale.h>
#include <Motion.h>
#include <stdlib.h>

#ifdef _WIN32
#define snprintf _snprintf
#define srandom srand
#define random rand
#endif

#define FRACSIZE 256
#define MAX_COLS 300


class FracGen: public Thread {

	public:

		FracGen(): Thread(), tex(FRACSIZE,FRACSIZE) {
			vx = -0.5; vy = 0; vs = 4; done = 0; changed = 1; max_iter = 75;
			fill_palette();
		}

		RGBATexture* load() {
			if (done && changed) {
				lock();
				tex.load( (&buffer), GL_RGB, GL_UNSIGNED_BYTE );
				changed = 0;
				release();
			}
			return &tex;
		}

		void setx( double _vx ) { vx -= _vx*vs; changed = 1; done = 0; }
		void sety( double _vy ) { vy += _vy*vs; changed = 1; done = 0; }
		void sets( double _vs ) { vs /= _vs;    changed = 1; done = 0; }

		void* run() {

			while (1) {

				if (done) { usleep(1000); continue; }

				lock();

				double tvx = vx;
				double tvy = vy;
				double tvs = vs;

				for (int x = 0; x < FRACSIZE; x++) for (int y = 0; y < FRACSIZE; y++) {

					buffer[y][x] = Color(0,0,0);

					// map texture coordinates to complex vector in unit square
					Vector c( (x-(FRACSIZE/2)), y-(FRACSIZE/2), 0 );
					c = c * (1.0/(double)FRACSIZE);

					// move & scale to desired viewpoint
					c = c * tvs;
					c.x += tvx;
					c.y += tvy;

					Vector z = c;
					
					//max_iter = 75*4/tvs;

					for (int i = 0; i < max_iter; i++) {
						z.set( z.x*z.x - z.y*z.y + c.x, 2*z.x*z.y + c.y, 0 );
						if ((z.x*z.x+z.y*z.y) > 4) {
							buffer[y][x] = palette[i*(MAX_COLS/max_iter)];
							break;
						}
					}
				}

				/*FILE* foo = fopen("tmp.pbm","w+");
				fprintf(foo,"P6 256 256 255 ");
				fwrite( buffer, FRACSIZE*FRACSIZE*3,1,foo );
				fclose(foo);*/

				release();
				done = 1;
			}

			return 0;
		}

		int done;

	protected:

		// generate a nice rainbow palette by rotating an
		// unit vector through the RG/GB/BR color subspaces
		void fill_palette( ) {
			const int count = MAX_COLS/3;
			for (int i = 0; i < count; i++) {
				Vector start(1,0,0);
				double angle = -i * M_PI/(2*count);
				start.rotate( angle ); start = start * 255;
				palette[i        ].set( start.x, start.y, 0 );
				palette[i+count  ].set( 0, start.x, start.y );
				palette[i+count*2].set( start.y, 0, start.x );
			}
		}

		RGBATexture tex;
		int changed,max_iter;
		Color buffer[FRACSIZE][FRACSIZE];
		Color palette[MAX_COLS];
		double vx,vy,vs;
};



class Mandelbrot: public Widget {

	public:

		Mandelbrot( int w, int h, int x = 0, int y = 0 ): Widget(w,h,x,y), fg() {

			region.gestures.push_back( Gesture( "scale" ) );
			region.gestures.push_back( Gesture( "move"  ) );
			region.flags( 1<<INPUT_TYPE_FINGER );

			fg.start();
		}

		void draw() {
		
			if (fg.done) { x = y = 0; sx = sy = 1; }
			mytex = fg.load();
			enter();
			glScalef(1.2,1.2,1.2);
			paint();
			leave();
		}

		void action( Gesture* gesture ) {

			if (gesture->name() == "scale") {
				FeatureBase* f = (*gesture)[0];
				Scale* r = dynamic_cast<Scale*>(f);
				if (!r) return;
				double res = r->result();
				sx *= res; 
				sy *= res; 
				fg.sets( res );
			}

			if (gesture->name() == "move") {
				FeatureBase* f = (*gesture)[0];
				Motion* r = dynamic_cast<Motion*>(f);
				if (!r) return;

				Vector tmp = r->result();
				transform( tmp );
				x += tmp.x;
				y += tmp.y;

				// movement in pixels -> movement in units
				fg.setx( (double)r->result().x / (double)w );
				fg.sety( (double)r->result().y / (double)h );
			}
		}

	protected:

		FracGen fg;

};


int main( int argc, char* argv[] ) {

	std::cout << "mandelbrot - TISCH widget layer demo 1.0 beta2" << std::endl;
	std::cout << "(c) 2009 by Florian Echtler <echtler@in.tum.de>" << std::endl;

	Window* win = new Window( 640, 640, "mandelbrot", ((argc > 1) ) );
	win->add( new Mandelbrot(640,640) );

	win->run();
}

