/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include <KinectImageSource.h>
#include <GLUTWindow.h>

KinectImageSource* ksrc = 0;
GLUTWindow* win = 0;
RGBImage rgb(640,480);
IntensityImage hue(640,480);
IntensityImage sat(640,480);
IntensityImage val(640,480);
IntensityImage tmp(640,480);
IntensityImage dsp(640,480);
IntensityImage sbl(640,480);
IntensityImage buf(160,120);

Image* disp = &tmp;

unsigned char thr  = 135;
unsigned char thr2 = 250;

int curframe = 0;
int lasttime = 0;
int lastframe = 0;

void mouse(int button, int state, int x, int y) {
	if (state == 0) return;
  //y = 480-y;
	std::cout << "hue: " << (int)hue.getPixel(x,y) << std::endl;
	std::cout << "sat: " << (int)sat.getPixel(x,y) << std::endl;
	std::cout << "val: " << (int)val.getPixel(x,y) << std::endl << std::endl;
}

void key(unsigned char code, int, int) {
	if (code == 'q') {
	  delete ksrc;
	  exit(1);
  }
	if (code == '1') disp = &rgb;
	if (code == '2') disp = &hue;
	if (code == '3') disp = &sat;
	if (code == '4') disp = &val;
	if (code == '5') disp = &tmp;
	if (code == '6') disp = &dsp;
	if (code == '7') disp = &sbl;
	if (code == '8') disp = &buf;

	if (code == '-') thr--;
	if (code == '+') thr++;
	if (code == '<') thr2--;
	if (code == '>') thr2++;

	std::cout << (int)thr << " " << (int)thr2 << std::endl;
	glutPostRedisplay();
}

void idle() {
	ksrc->acquire();
	ksrc->getImage( rgb );
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
	//std::cout << "doing image transform" << std::endl;
	rgb.getHSV(hue,sat,val);
	val.threshold(224);  val.mask(hue);
	//sat.threshold(40,90); sat.mask(hue);
	hue.threshold(thr,tmp,thr+30);
	tmp.despeckle(dsp,7);
	dsp.sobel(sbl);
	sbl.houghLine( buf );
	//std::cout << "hough transform done in " << (glutGet(GLUT_ELAPSED_TIME)-curtime) << " ms" << std::endl;

	if (disp == &rgb) win->show( rgb,0,0 );
  else win->show( *(IntensityImage*)disp, 0, 0 );

	glMatrixMode( GL_MODELVIEW );
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
	glPopMatrix();

	glutSwapBuffers();
}

int main(int argc, char* argv[]) {

	ksrc = new KinectImageSource();
	win = new GLUTWindow(640,480,"gldemo");

	glutDisplayFunc( display );
  glutKeyboardFunc(key);
  glutMouseFunc(mouse);
  glutIdleFunc(idle);

	ksrc->start();
	win->run();

	return 0;
}

