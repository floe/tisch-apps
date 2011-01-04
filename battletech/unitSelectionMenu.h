/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef UNITSELECTIONMENU_H
#define UNITSELECTIONMENU_H

#include "menu.h"

class unitSelectionMenu : public menu
{
public:
	unitSelectionMenu(int w, int h, int _number, int x, int y, double angle = 0.0, RGBATexture* tex = 0, int mode = 0x07 );

	void openUnitDataContainer(int num);
	void openUnitSelectionMenu(int num);
	void openPlayerMenu();
	void addUnits();
	void startDeploymentPhase();

	int number;
	std::vector<Checkbox*> unitlist;
};

#endif