/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef VICTORYMENU_H
#define VICTORYMENU_H

#include "menu.h"

class victoryMenu : public menu
{
public:
	victoryMenu(int w, int h, int draw, int x = 0, int y = 0, double angle = 0.0, RGBATexture* tex = 0, int mode = 0x07 );
};

#endif

