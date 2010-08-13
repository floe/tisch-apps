#include "EllipseHabitat.h"

#ifndef M_PI
	#define M_PI  3.14159265
#endif

EllipseHabitat::EllipseHabitat(int centerx, int centery, Vector major, Vector minor)
{
	centerX = centerx;
	centerY = centery;
	semimajoraxis = major;
	semiminoraxis = minor;

	majoraxislength = 2 * semimajoraxis.length();
	minoraxislength = 2 * semiminoraxis.length();

	angle = -acos(major.x / sqrt(major.x*major.x + major.y*major.y));

	this->calculateEllipse();
}

void EllipseHabitat::update()
{
	majoraxislength = 2 * semimajoraxis.length();
	minoraxislength = 2 * semiminoraxis.length();

	angle = -acos(semimajoraxis.x / sqrt(semimajoraxis.x*semimajoraxis.x + semimajoraxis.y*semimajoraxis.y));
}

void EllipseHabitat::calculateEllipse() 
{
	int steps = 72;
	//double beta = -angle / 180 * M_PI;
	double beta = -angle;
	double betarotinvar = 0;
	double sinbeta = sin(beta);
	double sinbetarotinvar = sin(betarotinvar);
	double cosbeta = cos(beta);
	double cosbetarotinvar = cos(betarotinvar);
	double a = this->getMajoraxislength() / 2;
	double b = this->getMinoraxislength() / 2;
	double x = this->getCenterX();
	double y = this->getCenterY();

	int j = 0;
	for (double i = 0; i < 360; i += 360.0 / (double)steps) 
	{
		double alpha = i / 180 * M_PI;
		double sinalpha = sin(alpha);
		double cosalpha = cos(alpha);

		double X = x + (a * cosalpha * cosbeta - b * sinalpha * sinbeta);
		double Y = y + (a * cosalpha * sinbeta + b * sinalpha * cosbeta);
		double Xrotinvar = x + (a * cosalpha * cosbetarotinvar - b * sinalpha * sinbetarotinvar);
		double Yrotinvar = y + (a * cosalpha * sinbetarotinvar + b * sinalpha * cosbetarotinvar);

		points[j] = X; pointsrotinvar[j] = Xrotinvar; j++;
		points[j] = Y; pointsrotinvar[j] = Yrotinvar; j++;
	}
}

int EllipseHabitat::contains(double x, double y, bool rotinvar) 
{
	Vector v(x,y);
	Vector v1;
	Vector v2;
	double tmp;
	int lcount = 0;
	//int rcount = 0;

	if(!rotinvar)
	{
		for(int i = 0; i < 144; i+=2)
		{
			//delete v1; delete v2;
			v1 = Vector(points[i], points[i+1]);
			if (i==142) 
				v2 = Vector(points[0], points[1]);
			else
				v2 = Vector(points[i+2], points[i+3]);

			if ((v1.y >  v.y) && (v2.y >  v.y)) continue;
			if ((v1.y <= v.y) && (v2.y <= v.y)) continue;

			tmp = v1.x + ( (v.y - v1.y) / (v2.y - v1.y) ) * (v2.x - v1.x);

			if (tmp < v.x) lcount++;
			//if (tmp > x) rcount++;
		}
	}
	else
	{
		for(int i = 0; i < 144; i+=2)
		{
			v1 = Vector(pointsrotinvar[i], pointsrotinvar[i+1]);
			if (i==142) 
				v2 = Vector(pointsrotinvar[0], pointsrotinvar[1]);
			else
				v2 = Vector(pointsrotinvar[i+2], pointsrotinvar[i+3]);

			if ((v1.y >  v.y) && (v2.y >  v.y)) continue;
			if ((v1.y <= v.y) && (v2.y <= v.y)) continue;

			tmp = v1.x + ( (v.y - v1.y) / (v2.y - v1.y) ) * (v2.x - v1.x);

			if (tmp < v.x) lcount++;
			//if (tmp > x) rcount++;
		}
	}
	return (lcount % 2);
}

Vector EllipseHabitat::getIntersectionpoint(double x0, double y0, double vecx, double vecy)
{
	Vector p1;
	Vector q1;
	Vector p2;
	Vector q2;
	double a1,b1,c1,a2,b2,c2;
	Vector s;
	bool result = true;
	p1.x = x0 - vecx; p2.x = x0 + 10000*vecx;
	p1.y = y0 - vecy; p2.y = y0 + 10000*vecy;
	for(int i = 0; i < 144; i= i+2)
	{
		q1.x = points[i]; 
		q1.y = points[i+1];
		q2.x = points[(i+2)%144]; 
		q2.y = points[(i+3)%144];

		a1 = p2.y - p1.y; b1 = p1.x - p2.x; c1 = a1*p1.x + b1*p1.y;
		a2 = q2.y - q1.y; b2 = q1.x - q2.x; c2 = a2*q1.x + b2*q1.y;

		s.x = (c1*b2-c2*b1)/(a1*b2-a2*b1); s.y = (a1*c2-a2*c1)/(a1*b2-a2*b1);

		result = true;
		if(((s.y > p1.y)&&(s.y > p2.y))
			|| ((s.y > q1.y) && (s.y > q2.y))
			|| ((s.y < p1.y) && (s.y < p2.y))
			|| ((s.y < q1.y) && (s.y < q2.y))
			|| ((s.x > p1.x) && (s.x > p2.x))
			|| ((s.x > q1.x) && (s.x > q2.x))
			|| ((s.x < p1.x) && (s.x < p2.x))
			|| ((s.x < q1.x) && (s.x < q2.x)))
				result = false;
		if(result)
		{
			break;
		}
	}
	if(result == false)
	{
		s.x = x0 + 100*vecx;
		s.y = y0 + 100*vecy;
	}
	return s;
}

void EllipseHabitat::drawEllipse()
{
	glColor4f(1.0,0,0,1);
	glLineWidth(1);
	glBegin(GL_LINE_LOOP);
		for(int i = 0; i < 144; i += 16)
			glVertex2d (points[i],points[i+1]);
	glEnd();	
}

void EllipseHabitat::setAxes(Vector major, Vector minor)
{
	semimajoraxis = major;
	semiminoraxis = minor;
	update();
}

void EllipseHabitat::setCenter(int centerx, int centery)
{
	centerX = centerx;
	centerY = centery;
	update();
}

