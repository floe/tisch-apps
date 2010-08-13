#ifndef _ELLIPSEHABITAT
#define _ELLIPSEHABITAT

#include "Habitat.h"
#include <Vector.h>
#include <GL/glut.h>

class EllipseHabitat : public Habitat
{
public:
	EllipseHabitat(int centerX, int centerY, Vector a, Vector b);

	void update();

	void calculateEllipse();
	int contains(double x, double y, bool rotinvar);
	void setAxes(Vector a, Vector b);
	void setCenter(int x, int y);
	double getMajoraxislength() const; double getMinoraxislength() const;
	int getCenterX() const; int getCenterY() const;
	double getAngle() const;
	Vector getIntersectionpoint(double x0, double y0, double vecx, double vecy);
	void drawEllipse();

private:
	int centerX, centerY;
	Vector semimajoraxis, semiminoraxis;
	double majoraxislength, minoraxislength;
	double points[144];
	double pointsrotinvar[144];
	double angle;
};

inline double EllipseHabitat::getMajoraxislength() const { return majoraxislength; }
inline double EllipseHabitat::getMinoraxislength() const { return minoraxislength; }
inline int EllipseHabitat::getCenterX() const { return centerX; }
inline int EllipseHabitat::getCenterY() const { return centerY; }
inline double EllipseHabitat::getAngle() const { return angle; }

#endif
