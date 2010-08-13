/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef DAMAGERESOLUTIONMENU_H
#define DAMAGERESOLUTIONMENU_H

#include "menu.h"

class damageResolutionMenu : public menu
{
public:
	damageResolutionMenu(int w, int h, int x = 0, int y = 0, double angle = 0.0, RGBATexture* tex = 0, int mode = 0xFF );

	void addWeaponFireString(int playernumber, int unitnumber);
	void addTargetString(int playernumber, int unitnumber);
	void addWeaponMissedString(int playernumber, int unitnumber, int weaponnumber);
	void addWeaponHitString(int playernumber, int unitnumber, int weaponnumber);
	void process_damage(int damage, int playernumber, int unitnumber);
	void showDestruction(int playernumber, int unitnumber);
	void draw();

	int lineposition, line, linespacing;
	std::string help;
	int location;
	std::string location_name;
	std::vector<Label*> labels;
	Label* round_label;
	Label* done_label;
	menubutton* done_button;
	Slider* slider;
	int slider_lines;
	double slider_pos;
};

#endif

