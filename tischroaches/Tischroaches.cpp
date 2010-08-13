/***************************************************************
 *                                                             *
 * program  : Tischroaches		                               *
 * author   : Andreas Dippon								   *
 * version  : Alpha 0.3                                        *
 * date     : 05.Dec.2008                                      *
 * modified : Jul-Aug 08: StaticObject, Habitat, Textures	   *
 *			  Aug-Sep 08: EllipseHabitat					   *
 *			  Oct-Dec 08: adapt to TISCHframework			   *
 * e-mail   : dippona@in.tum.de							       *
 *				                                               *
 *				                                               *
 ***************************************************************/
#include "Tischroaches.h"

GLuint texture[10];
RGBATexture* textures[10];
int texid = 0;
bool g_bSquarehab = false;

const int winwidth = 1024;
const int winheight = 768;

const int number = 30;

int mode = 0;
int deadcount = 0;

Roach* roaches[number];

int xbegin, ybegin;
int xleft, yleft;

#ifdef _MSC_VER
	int use_mouse = 1;
#else
	int use_mouse = 0;
#endif

Roachwindow* win;
//StaticObject* background;

//EllipseHabitat* ellipsehabi = new EllipseHabitat(200,-150,*(new Vector(-150,-150)), *(new Vector(50,50)));
//EllipseHabitat* ellipsehab[shadownumber];
std::map<int, EllipseHabitat*> ellipsehab;
int changecounter = 0;

SquareHabitat* squarehab = new SquareHabitat(200,200,0,0);
SquareHabitat* squarehab1 = new SquareHabitat(200,200,0,0);
SquareHabitat* squarehab2 = new SquareHabitat(200,200,0,0);
SquareHabitat* squarehab3 = new SquareHabitat(200,200,0,0);

void changeHabitatform()
{
	g_bSquarehab = !g_bSquarehab;
	for(int i = 0; i < number; i++)
		roaches[i]->squarehab = !roaches[i]->squarehab;
	if(g_bSquarehab)
		for(int i = 0; i < number; i++)
			roaches[i]->hab = squarehab;
	else
		for(int i = 0; i < number; i++)
			roaches[i]->hab = ellipsehab[0];
}

void mousefunc(int button, int state, int x, int y)
{
/*	if(button == 0 && state == GLUT_DOWN)
	{
		xleft = x;
		yleft = y;
	}
	if(button == 2 && state == GLUT_DOWN)
	{
		xbegin = x;
		ybegin = y;
	}
	if(button == 2 && state == GLUT_UP)
	{
		if(!g_bSquarehab)
				changeHabitatform();
		int width = fabs((float)(x-xbegin));
		int height = fabs((float)(y-ybegin));
		for(int i = 0; i < number; i++)
		{
			squarehab->setWidth(width);
			squarehab->setHeight(height);
			squarehab->setCenter((x-winwidth/2)-(x-xbegin)/2,(winheight/2-y)+(y-ybegin)/2);
		}
	}*/
}

void reset()
{
	for(int i = 0; i < number; i++)
		roaches[i]->reset();
}

void keyboard( unsigned char key, int x, int y ) {

	switch(key)
	{
	/*	case ' ':
			if(!g_bSquarehab)
				changeHabitatform();
			mode = !mode;
			if (mode) 
				for(int i = 0; i < number; i+=4) {
					squarehab->setWidth(3000);
					squarehab->setHeight(500);
					squarehab->setCenter(0,-800);
					roaches[i]->hab = squarehab;

					squarehab1->setWidth(3000);
					squarehab1->setHeight(500);
					squarehab1->setCenter(0,800);
					roaches[i+1]->hab = squarehab1;
			
					squarehab2->setWidth(500);
					squarehab2->setHeight(3000);
					squarehab2->setCenter(800,0);
					roaches[i+2]->hab = squarehab2;

					squarehab3->setWidth(500);
					squarehab3->setHeight(3000);
					squarehab3->setCenter(-800,0);
					roaches[i+3]->hab = squarehab3;
				}
			else
				for (int i = 0; i < number; i++) {
					squarehab->setWidth(300);
					squarehab->setHeight(400);
					squarehab->setCenter(-200,-200);
					roaches[i]->hab = squarehab;
				}
			break;
*/
		case '1':
			win->texture(0); break;
		case '2':
			win->texture(textures[3]); break;
		case '3':
			win->texture(textures[4]); break;
		case '4':
			win->texture(textures[5]); break;
		case '5':
			win->texture(textures[6]); break;
		case '6':
			win->texture(textures[7]); break;
		case '7':
			win->color(0,0,0,1); win->texture(0); break;
		case '8':
			win->texture(textures[8]); break;
		case 'r':
			reset(); break;

/*		case 'h':
			changeHabitatform();
			break;
*/
		case 'q':
			win->signOff();
			exit(0);
			break;

		default: break;
	}
}

void idlefunc()
{
	if(win->shadowchanged && win->shadowID != 0)
	{
		if(ellipsehab.count(win->shadowID) == 0)
		{
		//	if((win->shadowDim.x1*win->shadowDim.x1 + win->shadowDim.y1*win->shadowDim.y1) >= 
		//		(win->shadowDim.x2*win->shadowDim.x2 + win->shadowDim.y2*win->shadowDim.y2))
				ellipsehab[win->shadowID] = new EllipseHabitat(win->shadowPos.x-winwidth/2 ,win->shadowPos.y-winheight/2,
					Vector(win->shadowDim.x1,win->shadowDim.y1), Vector(win->shadowDim.x2,win->shadowDim.y2));
		//	else
		//		ellipsehab[win->shadowID] = new EllipseHabitat(win->shadowPos.x-winwidth/2 ,win->shadowPos.y-winheight/2,
		//			*(new Vector(-win->shadowDim.x2,win->shadowDim.y2)), *(new Vector(win->shadowDim.x1,win->shadowDim.y1)));
		}
		else
		{
			ellipsehab[win->shadowID]->setCenter(win->shadowPos.x-winwidth/2 ,win->shadowPos.y-winheight/2);
		//	if((win->shadowDim.x1*win->shadowDim.x1 + win->shadowDim.y1*win->shadowDim.y1) >= 
		//		(win->shadowDim.x2*win->shadowDim.x2 + win->shadowDim.y2*win->shadowDim.y2))
				ellipsehab[win->shadowID]->setAxes( Vector(win->shadowDim.x1,win->shadowDim.y1),
					Vector(win->shadowDim.x2,win->shadowDim.y2));
		//	else
		//		ellipsehab[win->shadowID]->setAxes(*(new Vector(-win->shadowDim.x2,win->shadowDim.y2)),
		//			*(new Vector(win->shadowDim.x1,win->shadowDim.y1)));

			//ellipsehab[win->shadowID]->setAxes(*(new Vector(-30,20)), *(new Vector(20,10)));
			ellipsehab[win->shadowID]->calculateEllipse();
		}
		win->shadowchanged = false;				
	}
	if(win->shadowerased && win->shadowID != 0)
	{
		ellipsehab.erase(win->shadowID);
		win->shadowerased = false;
	}
		
	if (ellipsehab.size() != 0) 
	for(int i=0; i < number; i++)
	{
		if(roaches[i]->changecounter == changecounter)
		{	
			int help = rand() % ellipsehab.size();
			int help1 = 0;
			if(ellipsehab.size() > 8) help = (rand() % (ellipsehab.size()-8)) + 8;
			for (std::map<int,EllipseHabitat*>::iterator it = ellipsehab.begin(); it != ellipsehab.end(); it++) 
			{
				if(help == help1)
				{
					roaches[i]->hab = it->second; break;
				}
				else
					help1++;
			}
		}	
	}

	changecounter++;
	if(changecounter == 500)
	{
		for(int i = 0; i<number; i++)
			roaches[i]->changecounter = 50+rand()%50;
		changecounter = 0;
	}
	//win->update();
	win->idle();
}

int main(int argc, char **argv) {

	srand(time(0)*time(0));

	win = new Roachwindow(winwidth,winheight,"Tischroaches - press r to reanimate, 1-8 to change background",use_mouse);

	ellipsehab[0] = new EllipseHabitat(700,-700,*(new Vector(100,100)), *(new Vector(50,-50)));
	ellipsehab[1] = new EllipseHabitat(700,700,*(new Vector(100,100)), *(new Vector(50,-50)));
	ellipsehab[2] = new EllipseHabitat(-700,-700,*(new Vector(100,100)), *(new Vector(50,-50)));
	ellipsehab[3] = new EllipseHabitat(-700,700,*(new Vector(100,100)), *(new Vector(50,-50)));
	ellipsehab[4] = new EllipseHabitat(0,700,*(new Vector(100,100)), *(new Vector(50,-50)));
	ellipsehab[5] = new EllipseHabitat(0,-700, *(new Vector(100,100)), *(new Vector(50,-50)));
	ellipsehab[6] = new EllipseHabitat(700,0, *(new Vector(100,100)), *(new Vector(50,-50)));
	ellipsehab[7] = new EllipseHabitat(-700,0, *(new Vector(100,100)), *(new Vector(50,-50)));
	
	textures[0] = new RGBATexture("../Tischroaches/roach.png"); std::cout << "loaded texture 1" << std::endl;
	textures[1] = new RGBATexture("../Tischroaches/roach2.png"); std::cout << "loaded texture 2" << std::endl;
	textures[2] = new RGBATexture("../Tischroaches/splat.png"); std::cout << "loaded texture 3" << std::endl;
	textures[3] = new RGBATexture("../Tischroaches/holztisch.png"); std::cout << "loaded texture 4" << std::endl;
	textures[4] = new RGBATexture("../Tischroaches/fliese.png"); std::cout << "loaded texture 5" << std::endl;
	textures[5] = new RGBATexture("../Tischroaches/holz.png"); std::cout << "loaded texture 6" << std::endl;
	textures[6] = new RGBATexture("../Tischroaches/metall.png"); std::cout << "loaded texture 7" << std::endl;
	textures[7] = new RGBATexture("../Tischroaches/gras.png"); std::cout << "loaded texture 8" << std::endl;
	textures[8] = new RGBATexture("../Tischroaches/Desktop.png"); std::cout << "loaded texture 9" << std::endl;

	//background = new StaticObject(textures[3],winwidth, winheight);
	win->texture(textures[3]);

	for(int i = 0; i < number; i++)
	{
		roaches[i] = new Roach(textures[0],40,40,0,0);
		if(g_bSquarehab)
		{
			roaches[i]->hab = squarehab;
			roaches[i]->squarehab = true;
		}
		else
		{
			roaches[i]->hab = ellipsehab[0];
			roaches[i]->squarehab = false;
		}
		roaches[i]->texture1 = textures[0];
		roaches[i]->texture2 = textures[1];
		roaches[i]->texture3 = textures[2];
		roaches[i]->speed = 0.001;
		win->add(roaches[i]);
	}

//	glutMouseFunc(mousefunc);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idlefunc);

	glEnable(GL_DEPTH_TEST);

	win->update();
	win->run();
}
