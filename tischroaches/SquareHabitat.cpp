#include "SquareHabitat.h"

SquareHabitat::SquareHabitat(int w, int h, int x, int y)
{
	width = w;
	height = h;
	centerX = x;
	centerY = y;
	update();
}

void SquareHabitat::update()
{
	up = centerY + 0.5f * height;
	down = centerY - 0.5f * height;
	right = centerX + 0.5f * width;
	left = centerX - 0.5f * width;
}

void SquareHabitat::setHeight(int h)
{
	height = h;
	update();
}
void SquareHabitat::setWidth(int w)
{
	width = w;
	update();
}
void SquareHabitat::setCenter(int x, int y)
{
	centerX = x;
	centerY = y;
	update();
}

