/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef MAINMENU_H
#define MAINMENU_H

#include "menu.h"

class mainMenu : public menu
{
public:
	mainMenu(int w, int h, int x, int y, double angle = 0.0, RGBATexture* tex = 0, int mode = 0x07 );

	void openPlayerMenu();
};

#endif