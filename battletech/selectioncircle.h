/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef SELECTIONCIRCLE_H
#define SELECTIONCIRCLE_H

#include <tisch.h>
#include <Tile.h>
#include <Texture.h>
#include "textures.h"

class selectioncircle : public Tile
{
public:
	selectioncircle(bool target, float tilewidth, float x = 0, float y = 0, float w = 0, float h = 0, double angle = 0, RGBATexture* tex = 0);

	void draw();
	void outline();
	void setCoordinates(int x, int y);

	bool targetcircle;
};

#endif

