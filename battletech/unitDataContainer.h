/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef UNITDATACONTAINER_H
#define UNITDATACONTAINER_H

#include <tisch.h>
#include <widgets/Container.h>
#include <widgets/Label.h>
#include "textures.h"
#include "unit.h"

class unitDataContainer : public Container
{
public:
	unitDataContainer(int w, int h, int unittype, bool _attacker = false, int x = 0, int y = 0, double angle = 0.0, RGBATexture* tex = 0, int mode = 0xFF);

	void draw();
	void drawMissingHitpoints();

	void drawUnit_Enforcer(unit* unit); //draw missing hitpoints for "ENF-4R Enforcer"
	void drawUnit_Hunchback(unit* unit); //draw missing hitpoints for "HBK-4G Hunchback"

private:
	int type;
	bool attacker; //needed to choose between selectedUnit or selectedTarget
	double start_x, start_y, dist_x, dist_y;
	double d; //length of circle-diameter
};

#endif
