
#include "tisch.h"
#include "Button.h"
#include "Habitat.h"
#include "SquareHabitat.h"
#include "EllipseHabitat.h"
#include <algorithm>
#include <iostream>
using namespace std;

class Roach : public Button
{
public:
	Roach(RGBATexture* tex, int _w, int _h, int _x = -1, int _y = -1, double angle = 0.0);
    void draw();
	void randomMove();
	//void action(Gesture *gesture);
	void tap(Vector pos, int id);
	void reset();

	RGBATexture* texture1;
	RGBATexture* texture2;
	RGBATexture* texture3;
	float speed;
	bool alive;
	
	double getX() const; double getY() const;
	double getWidth() const; double getHeight() const;

	Habitat* hab;
	bool squarehab;

	int changecounter;
	
private:

	int texnr;
	int i;
	bool pointreached;
	double pointX, pointY;
	float red,green,blue;
	float P0x, P0y, P1x, P1y, P2x, P2y, P3x, P3y;
	float t;
	float xalt, yalt;
	float savedangle;
};

inline double Roach::getX() const { return x; }
inline double Roach::getY() const { return y; }
inline double Roach::getWidth() const { return w; }
inline double Roach::getHeight() const { return h; }


