/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "unittile.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "gametile.h"

unittile::unittile(float x, float y, float w, float h, double angle, RGBATexture* tex) : Button(w,h,x,y,angle,tex){}

void unittile::draw()
{
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	Widget::draw();
}

void unittile::outline(){};

void unittile::setTexture(RGBATexture* tex)
{
	mytex = tex;
}

void unittile::setFacing(int num)
{
	switch(num)
	{
	case 0: angle = 330; break;
	case 1: angle = 270; break;
	case 2: angle = 210; break;
	case 3: angle = 150; break;
	case 4: angle = 90; break;
	case 5: angle = 30; break;
	}
	angle = angle * 2 * M_PI/360;
}
