/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef UNITLISTMENU_H
#define UNITLISTMENU_H

#include "menu.h"

class unitListMenu : public menu
{
public:
	unitListMenu(int w, int h, int _playernumber, int x = 0, int y = 0, double angle = 0.0, RGBATexture* tex = 0, int mode = 0x07 );

	virtual void draw();
	void openUnitDataContainer(int num);
	void removeUnit();
	void updateUnitList();
	void clearList();
	void revertDeployment();

	std::vector<Checkbox*> checkboxlist;
	int playernumber;

private:
	int lastSize;
	int activeCheckbox;
	std::vector<Label*> unitLabel;
	std::vector<menubutton*> unitButton;
};

#endif

