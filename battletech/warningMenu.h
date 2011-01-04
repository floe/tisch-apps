/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef WARNINGMENU_H
#define WARNINGMENU_H

#include "menu.h"

class warningMenu : public menu
{
public:
	warningMenu(int w, int h, int x = 0, int y = 0, double angle = 0.0, RGBATexture* tex = 0, int mode = 0x07 );
	void setText(std::string str);
	//void outline();
	void draw();

private:
	Label* myLabel;
};

#endif

