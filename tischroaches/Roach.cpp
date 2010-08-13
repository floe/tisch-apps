#include "Roach.h"
#include <BlobID.h>
#include <BlobPos.h>

#ifndef M_PI
	#define M_PI  3.14159265
#endif

Roach::Roach(RGBATexture* tex, int _w, int _h, int _x, int _y, double angle) : Button(_w,_h, _x,_y, angle, tex)
{
	i=3;
	pointreached = false;

	P0x = 0;P0y = 0;
	P1x = 0;P1y = 100;
	P2x = -50;P2y = 100;
	P3x = -100;P3y = 0;
	t = (float)rand() / RAND_MAX;

	speed = 1; //change speed when creating the roach (end of Tischroaches.cpp)
	alive = 1;

	texnr = 0;

	changecounter = 50+rand()%50;

	//hab = new SquareHabitat();
	hab = new EllipseHabitat(-100,150,*(new Vector(100,100)), *(new Vector(50,-50)));
	squarehab = false;

	region.flags( (1<<INPUT_TYPE_FINGER) | REGION_FLAGS_VOLATILE );
	region.gestures.clear();

	BlobID*  bid  = new BlobID();
	BlobPos* bpos = new BlobPos();

	Gesture tap( "tap", GESTURE_FLAGS_ONESHOT );
	tap.push_back( bid );
	tap.push_back( bpos );

	region.gestures.push_back( tap );
}

void Roach::draw()
{	
	if(speed != 0)
	{
		randomMove();
	}
	/*
	//Wegpunkte einzeichnen
	glColor4f(0,1.0,1.0,1.0);
	glBegin(GL_LINE_STRIP);
		glVertex2d(P0x, P0y);
		glVertex2d(P1x, P1y);
		glVertex2d(P2x, P2y);
		glVertex2d(P3x, P3y);
	glEnd();
	*/
	/*if(!squarehab)
		((EllipseHabitat*)hab)->drawEllipse();
	else
	{
		glColor4f(1.0,0,0,1.0);
		glLineWidth(4);
		glBegin(GL_LINE_LOOP);
			glVertex2d(((SquareHabitat*)hab)->getLeft(), ((SquareHabitat*)hab)->getDown());
			glVertex2d(((SquareHabitat*)hab)->getRight(), ((SquareHabitat*)hab)->getDown());
			glVertex2d(((SquareHabitat*)hab)->getRight(), ((SquareHabitat*)hab)->getUp());
			glVertex2d(((SquareHabitat*)hab)->getLeft(), ((SquareHabitat*)hab)->getUp());
		glEnd();
		glLineWidth(1);
	}
*/

	//enter(alive);
	enter();

	glColor4f( 1.0, 1.0, 1.0, 1.0 );

	if(!alive)
		mytex = texture3;
	else
	{
		if(texnr < 5)
			mytex = texture1;
		else if(texnr < 10)
			mytex = texture2;
		texnr++;
		texnr %= 10;
	}

	paint();
	leave();
}

void Roach::tap(Vector pos, int id)
{
	alive = 0;
	speed = 0;
	region.flags(0);
	lower(this);
}

void Roach::reset()
{
	alive = 1;
	speed = 0.001;
	region.flags( (1 << INPUT_TYPE_FINGER) | REGION_FLAGS_VOLATILE );
}

void Roach::randomMove()
{
	xalt = x;
	yalt = y;
	x = (1-t)*(1-t)*(1-t) * P0x + 3*(1-t)*(1-t) * t * P1x + 3*(1-t)*t*t*P2x + t*t*t * P3x;
	y = (1-t)*(1-t)*(1-t) * P0y + 3*(1-t)*(1-t) * t * P1y + 3*(1-t)*t*t*P2y + t*t*t * P3y;
	float vecx = x-xalt;
	float vecy = y-yalt;
	float veclength = sqrt(vecx*vecx + vecy*vecy);
	angle = acos(vecy/veclength);
	angle = -angle;

	if(vecx < 0 && vecy > 0)
	{
		angle = -angle;
	}
	else if(vecx < 0 && vecy < 0)
	{
		angle = 2*M_PI - angle;
	}
	
	if(i <= 2)
	{
		angle = savedangle;
		i++;
	}
	
	t += speed;
	if(t>1) t = 1;
	if(pointreached)
	{
		int sign = 0;
		if(rand()%2 == 0)
			sign = -1;
		else
			sign = 1;
		pointreached = false;
		if(squarehab) //typeof(hab) == SquareHabitat ?
		{
			P0x = xalt; P0y = yalt;
			P1x = max(((SquareHabitat*)hab)->getLeft(), min(((SquareHabitat*)hab)->getRight(), xalt+P1x*200)); 
			P1y = max(((SquareHabitat*)hab)->getDown(), min(((SquareHabitat*)hab)->getUp(), yalt+P1y*200));
			P2x = max(((SquareHabitat*)hab)->getLeft(), min(((SquareHabitat*)hab)->getRight(), P1x + sign * P2x*200)); 
			P2y = max(((SquareHabitat*)hab)->getDown(), min(((SquareHabitat*)hab)->getUp(), P1y + sign * P2y*200));
			P3x = ((SquareHabitat*)hab)->getLeft() + rand() % (int)((SquareHabitat*)hab)->getWidth(); 
			P3y = ((SquareHabitat*)hab)->getDown() + rand() % (int)((SquareHabitat*)hab)->getHeight();
		}
		else 
		{
			P0x = xalt; P0y = yalt;
			vecx = P1x;
			vecy = P1y;
			//Ellipsengleichung: (x-x0)^2 / a^2  +  (y-y0)^2 / b^2 = 1   Hauptachse parallel zur x-Achse
			if(vecx != 0)
			{
				//P1 berechnen...
				Vector help = ((EllipseHabitat*)hab)->getIntersectionpoint(P0x, P0y, vecx, vecy);
				P1x = help.x; P1y = help.y;
				//P1 berechnet--- 

				//P2 berechnen...
				help = ((EllipseHabitat*)hab)->getIntersectionpoint(P1x, P1y, P2x, P2y);
				P2x = help.x; P2y = help.y;
				//P2 berechnet---

				//P3 zufällig in der Ellipse
				do {
					P3x = ((EllipseHabitat*)hab)->getCenterX() + rand() % (int)((EllipseHabitat*)hab)->getMajoraxislength() 
						- ((EllipseHabitat*)hab)->getMajoraxislength()/2;
					P3y = ((EllipseHabitat*)hab)->getCenterY() + rand() % (int)((EllipseHabitat*)hab)->getMinoraxislength() 
						- ((EllipseHabitat*)hab)->getMinoraxislength()/2;
				}while(((EllipseHabitat*)hab)->contains(P3x,P3y, true) == 0);
				if(((EllipseHabitat*)hab)->getAngle() != 0)
				{
					double u = -((EllipseHabitat*)hab)->getAngle() + atan2(P3y-((EllipseHabitat*)hab)->getCenterY(),
						P3x - ((EllipseHabitat*)hab)->getCenterX());
					double d = sqrt((P3x-((EllipseHabitat*)hab)->getCenterX())*(P3x-((EllipseHabitat*)hab)->getCenterX()) + 
						  (P3y-((EllipseHabitat*)hab)->getCenterY())*(P3y-((EllipseHabitat*)hab)->getCenterY()));
					P3x = ((EllipseHabitat*)hab)->getCenterX()+d*cos(u);
					P3y = ((EllipseHabitat*)hab)->getCenterY()+d*sin(u);
				}
				//P3 berechnet---
			}
			else
			{
				P1x = 0; P1y = 0; P2x = 0; P2y = 0; P3x = 0; P3y = 0;
			}
		}
		x = P0x;
		y = P0y;
		
	}
	else
	{
		if(fabs(P3x - x) < 5 && fabs(P3y - y) < 5)
		{
			pointreached = true;
			P1x = vecx/veclength;
			P1y = vecy/veclength;
			P2y = sqrt(1/((P1y/P1x)*(P1y/P1x)+1));
			P2x = -1* P2y * P1y / P1x;
			t = 0;
			i = 0;
		}
		else
			savedangle = angle;
	}	
}


