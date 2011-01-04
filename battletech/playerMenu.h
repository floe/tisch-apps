/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef PLAYERMENU_H
#define PLAYERMENU_H

#include "menu.h"

class playerMenu : public menu
{
public:
	playerMenu(int w, int h, int x = 0, int y = 0, double angle = 0.0, RGBATexture* tex = 0, int mode = 0x07);

	void openUnitSelectionMenu();
	void openUnitListMenu();
	void openMainMenu();
	void changeNumber();

	int num_of_players;

private:
	Label* number;
};

#endif
