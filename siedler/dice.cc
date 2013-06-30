/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <stdio.h>
#include <deque>

#include <KinectImageSource.h>
#include <GLUTWindow.h>
#include <Texture.h>
#include <Blob.h>

KinectImageSource* ksrc = 0;
GLUTWindow* win = 0;

RGBImage rgb(1280,1024);

ShortImage depth(640,480);
ShortImage diff(640,480);
ShortImage thre(640,480);
ShortImage desp(640,480);
ShortImage bg(640,480);

RGBATexture* numbers[13];
RGBATexture* number = NULL;
std::deque<int> rolls;

bool shownum = true;

IntensityImage mask(640,480);

Image* disp = &rgb;

std::vector<Blob> blobs;

int thr = 230;

int curframe = 0;
int lasttime = 0;
int lastframe = 0;

void mouse(int button, int state, int x, int y) {
	if (state == 0) return;
  //y = 480-y;
}

void key(unsigned char code, int, int) {
	if (code == 'q') {
	  delete ksrc;
	  exit(1);
  }
	if (code == '1') disp = &rgb;
	if (code == '2') disp = &depth;
	if (code == '3') disp = &bg;
	if (code == '4') disp = &thre;
	if (code == '5') disp = &desp;
	if (code == '6') disp = &mask;
	if (code == '7') disp = NULL;

	if (code == '-') thr--;
	if (code == '+') thr++;

	if (code == ' ') bg = depth;
	if (code == 'f') glutFullScreen();
	if (code == 'n') shownum = !shownum;

	std::cout << (int)thr << " " << std::endl;
	glutPostRedisplay();
}

void idle() {
	ksrc->acquire();
	ksrc->getImage( rgb );
	ksrc->getImage( depth );
	ksrc->release();
	glutPostRedisplay();
	if (curframe++ == 10) bg = depth;
}

void display() {

	/*if ((curtime - lasttime) >= 1000) {
		double fps = (1000.0*(curframe-lastframe))/((double)(curtime-lasttime));
		lasttime  = curtime;
		lastframe = curframe++;
		std::cout << "fps: " << fps << std::endl;
	}*/
	
	win->clear();
	win->mode2D();

	int curtime = glutGet( GLUT_ELAPSED_TIME );
	/*val.threshold(224);
	tmp.despeckle(dsp,7);
	dsp.sobel(sbl);
	sbl.houghLine( buf );*/
	bg.subtract( depth, diff, 0 );
	diff.threshold(150,thre,3000);
	thre.despeckle(desp,4);
	desp.convert( mask );

	unsigned char* rgbdata = rgb.getData();
	unsigned char* maskdata = mask.getData();

	unsigned char value = 254;
	int gid = 1;

	blobs.clear();

	if (!disp && !shownum) win->show( rgb,0,0 );

	for (int i = 0; i < 640*480; i++) if (maskdata[i] == 255) try {

		// try to create a new blob. throws if blob too small, continues silently.
		blobs.push_back( Blob( &mask, Point(i%640,i/640), value, gid, 200, 800) );

		// adjust counters
		value--;
		gid++;

		// did the frame-local blob counter overflow?
		if (value == 0) {
			value = 254;
			std::cerr << "Warning: too many blobs!" << std::endl;
		}

	} catch (...) { }

	int xoff = 0, yoff = 0, sum = 0;

	glTranslatef(0,0,100);
	// for each remaining blob:
	for (std::vector<Blob>::iterator blob = blobs.begin(); blob != blobs.end(); blob++) {

		// create IntensityImage(50,50)
		IntensityImage diceimg(50,50);
		unsigned char* dicedata = diceimg.getData();

		// copy pixels from rgbdata at adjusted blob position
		int cx = blob->pos.x * 2 + xoff;
		int cy = blob->pos.y * 2 + yoff;

		for (int y = 0; y < 50; y++) for (int x = 0; x < 50; x++) {
			int sx = cx - 25 + x;
			int sy = cy - 25 + y;
			int i = (sy*1280+sx)*3;
			int res = rgbdata[i] + rgbdata[i+1] + rgbdata[i+2];
			dicedata[y*50+x] = res/3;
			rgbdata[i] = rgbdata[i+1] = rgbdata[i+2] = res/3;
		}

		//win->show( diceimg, cx-25, cy-25 );

		// get avg. intensity
		diceimg.invert();
		diceimg.threshold(thr);
		IntensityImage tmp(50,50);
		diceimg.despeckle(tmp,7);
		diceimg = tmp;
		//int intensity = diceimg.intensity();

		// invert, apply as threshold

		for (int y = 0; y < 50; y++) for (int x = 0; x < 50; x++) {
			int sx = cx - 25 + x;
			int sy = cy - 25 + y;
			int i = (sy*1280+sx)*3;
			rgbdata[i] = rgbdata[i+1] = rgbdata[i+2] = dicedata[y*50+x];
		}

		std::vector<Blob> eyes;
		unsigned char eyeval = 254;
		int eyegid = 1;

		// find blobs ~ 50 px
		for (int i = 0; i < 50*50; i++) if (dicedata[i] == 255) try {
			eyes.push_back( Blob( &diceimg, Point(i%50,i/50), eyeval, eyegid, 5, 30) );
			value--; gid++;
		} catch (...) { }

		// if count in [1,6] -> würfel
		if ((eyes.size() > 0) && (eyes.size() < 7)) {
			char textbuf[1024]; snprintf(textbuf,1024,"%d",eyes.size());
			glColor3f(1,0,0);
			if (shownum) win->print(textbuf,cx*800/1280,cy*600/1024);
			sum += eyes.size();
		}
	}
	glTranslatef(0,0,-100);

	// check if same sum for last 10 frames
	rolls.push_back(sum);
	if (rolls.size() > 10) rolls.pop_front();

	int firstsum = rolls[0];
	if (rolls.size() == 10)
		for (int i = 1; i < 10; i++)
			if (rolls[i] != firstsum) firstsum = -1; 

	if ((firstsum >= 2) && (firstsum <= 12))
		number = numbers[firstsum];
	else
		number = numbers[0];

	if (number) {
		number->bind();
		number->render();
		number->release();
	}

	/*masked.clear();

	for (int y = 0; y < 960; y++)
	for (int x = 0; x < 1280; x++) {
		int i = ((y+yoff)>>1)*640+(x>>1);
		if (i<0 || depthdata[i] == 0) continue;
		i = y*1280+x;
		maskdata[i*3+0] = rgbdata[i*3+0];
		maskdata[i*3+1] = rgbdata[i*3+1];
		maskdata[i*3+2] = rgbdata[i*3+2];
	}*/

	if (disp)  { if (disp == &rgb) win->show( *(RGBImage*)disp,0,0 );
  else if (disp == &mask) win->show( *(IntensityImage*)disp, 0, 0 );
  else win->show( *(ShortImage*)disp, 0, 0 ); }

	/*glMatrixMode( GL_MODELVIEW );
	glColor3f(1,0,0);
	glPushMatrix();
	glTranslatef( 0, 0, 500 ); // 500 = 0.5 in Texture::render()
	glBegin(GL_LINES);
	for (int x = 0; x < 160; x++) for (int y = 0; y < 120 ; y++) {
		if (buf.getPixel(x,y) < thr2) continue;
		float theta = M_PI*((float)x/160.0);
		float rho   = ((float)y/120.0 - 0.5)*2*sqrtf(640*640+480*480);
		glVertex2f( 0, 480-rho/sinf(theta) );
		glVertex2f( 640, 480-(rho-640*cosf(theta))/sinf(theta) );
	}
	glEnd();
	glPopMatrix();*/

	glutSwapBuffers();
}

int main(int argc, char* argv[]) {

	ksrc = new KinectImageSource();
	win = new GLUTWindow(800,600,"gldemo");

	numbers[0] = new RGBATexture("siedler.png");
	for (int i = 2; i <= 12; i++) {
		char namebuf[100];
		snprintf(namebuf,100,"siedler%02d.png",i);
		numbers[i] = new RGBATexture(namebuf);
	}

	glutDisplayFunc( display );
  glutKeyboardFunc(key);
  glutMouseFunc(mouse);
  glutIdleFunc(idle);

	ksrc->setHiRes(true);
	ksrc->start();
	win->run();

	return 0;
}

