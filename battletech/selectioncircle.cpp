/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "selectioncircle.h"
#include "engine.h"

selectioncircle::selectioncircle(bool target, float tilewidth, float x, float y, float w, float h, double angle, RGBATexture* tex) : Tile(w,h,x,y,angle,tex,0)
{
	targetcircle = target;
	if(!targetcircle)
		texture(tex_storage.getSelectionTexture(1));
	else
		texture(tex_storage.getSelectionTexture(2));
	this->w = 3.2 * tilewidth;
	this->h = this->w;
}

void selectioncircle::draw()
{
	if((gameengine.field->selectedUnit != 0  && !targetcircle) || (gameengine.field->selectedTargetUnit != 0 && targetcircle))
	{	
		angle += M_PI / 60;
		if(angle > 2*M_PI) angle = 0;
		glColor4f(1,1,1,1);
		Tile::draw();
	}
}

void selectioncircle::setCoordinates(int x, int y)
{
	this->x = x;
	this->y = y;
}

void selectioncircle::outline(){}
