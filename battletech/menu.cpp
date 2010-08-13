/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "menu.h"
#include "warningMenu.h"

menu::menu(int w, int h, int x, int y, double angle, RGBATexture* tex, int mode) :
	Container(w, h, x, y, angle, tex, mode) 
{}

	
void menu::dropWarning(std::string str)
{
	gameengine.activeWarning = true;
	gameengine.warnMenu = new warningMenu(600,100);
	gameengine.warnMenu->setText(str);
	((Container*)this->parent)->add(gameengine.warnMenu);
}
