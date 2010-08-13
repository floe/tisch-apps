/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef INFOMENU_H
#define INFOMENU_H

#include "menu.h"

class infoMenu : public menu
{
public:
	infoMenu(int w, int h, int x = 0, int y = 0, double angle = 0.0, RGBATexture* tex = 0, int mode = 0xFF );

	void displayUnitInfo(unit* unit);
	void displayTargetInfo(unit* unit);
	void resetAll();
	void resetTarget();
	void set_hit_prob_text();
	void draw();
	void fire_weapon();

	unit* myunit;
	Label* unitLabel;
	menubutton* unitButton;
	menubutton* targetButton;
	menubutton* fireButton;
	Label* infotext;
	Label* hit_prob_text;
	Label* fire_text;
	Label* fired_text;
	Label* ammo_text;
	std::vector<Label*> weapons;
	std::vector<Checkbox*> checkboxlist;
	int activeCheckbox, number_of_checkboxes;
	bool unit_selected, target_selected, target_locked;
};

#endif

