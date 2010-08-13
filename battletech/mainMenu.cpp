/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "mainMenu.h"
#include "playerMenu.h"

mainMenu::mainMenu(int w, int h, int x, int y, double angle, RGBATexture* tex, int mode) : 
	menu(w, h, x, y, angle, tex, mode)
{
	mytex = tex_storage.getBattletechTexture();

	double bw = 0.3*w;
	double bh = h/12;
	double bX = 0.33*w;
	double bY = -2*h/6 + 10;

	this->add(new menubutton(bw, bh, 3, bX, bY));
	this->add(new Label("New Game", bw, bh, bX, bY));
	this->add(new menubutton(bw, bh, 0, bX, bY - h/12 - 6));
	this->add(new Label("Exit", bw/2, bh, bX, bY - h/12 - 6));
}

void mainMenu::openPlayerMenu()
{
	((Container*)this->parent)->add(new playerMenu(300,100));
}
