/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef UNITTILE_H
#define UNITTILE_H

#include <tisch.h>
#include <widgets/Button.h>
#include <Texture.h>
#include "textures.h"
#include "unit.h"

class unittile : public Button
{
public:
	unittile(float x, float y, float w, float h, double angle = 0, RGBATexture* tex = 0);

	void draw();
	void outline();
	void setTexture(RGBATexture* tex);
	void setFacing(int num);
};

#endif

