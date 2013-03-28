/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef MENUBUTTON_H
#define MENUBUTTON_H

#include <tisch.h>
#include <Button.h>
#include "engine.h"

class menubutton : public Button
{
public:
	menubutton(int w, int h, int buttontype, int x = 0, int y = 0, double angle = 0.0, RGBATexture* tex = 0);

	void setType(int num); //sets type to num
	void tap(Vector pos, int id); //action depends on type

private:
	int type;
};

#endif
