/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <KinectImageSource.h>
#include <GLUTWindow.h>

KinectImageSource* ksrc = 0;
GLUTWindow* win = 0;

RGBImage rgb(1280,1024);
RGBImage masked(1280,1024);

ShortImage depth(640,480);
ShortImage diff(640,480);
ShortImage thre(640,480);
ShortImage desp(640,480);
ShortImage bg(640,480);

Image* disp = &rgb;

int thr = 150;

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
	if (code == '4') disp = &desp;
	if (code == '6') disp = &masked;

	if (code == '-') thr--;
	if (code == '+') thr++;

	if (code == ' ') bg = depth;

	std::cout << (int)thr << " " << std::endl;
	glutPostRedisplay();
}

void idle() {
	ksrc->acquire();
	ksrc->getImage( rgb );
	ksrc->getImage( depth );
	ksrc->release();
	glutPostRedisplay();
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
	diff.threshold(thr,thre,3000);
	thre.despeckle(desp,4);

	masked.clear();

	unsigned char* rgbdata = rgb.getData();
	unsigned char* maskdata = masked.getData();
	unsigned short* depthdata = (unsigned short*)desp.getData();

	int xoff = 0, yoff = 5;

	for (int y = 0; y < 960; y++)
	for (int x = 0; x < 1280; x++) {
		int i = ((y+yoff)>>1)*640+(x>>1);
		if (i<0 || depthdata[i] == 0) continue;
		i = y*1280+x;
		maskdata[i*3+0] = rgbdata[i*3+0];
		maskdata[i*3+1] = rgbdata[i*3+1];
		maskdata[i*3+2] = rgbdata[i*3+2];
	}

	if (disp == &rgb || disp == &masked) win->show( *(RGBImage*)disp,0,0 );
  else win->show( *(ShortImage*)disp, 0, 0 );

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
	win = new GLUTWindow(1280,1024,"gldemo");

	glutDisplayFunc( display );
  glutKeyboardFunc(key);
  glutMouseFunc(mouse);
  glutIdleFunc(idle);

	ksrc->start();
	win->run();

	return 0;
}

