/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef MENU_H
#define MENU_H

#include <tisch.h>
#include <widgets/Container.h>
#include <widgets/Checkbox.h>
#include <widgets/Textbox.h>
#include <widgets/Dial.h>
#include <widgets/Slider.h>
#include <widgets/Label.h>
#include "menubutton.h"
#include "textures.h"
#include "engine.h"

class menu : public Container
{
public:
	menu(int w, int h, int x, int y, double angle, RGBATexture* tex, int mode);

	virtual void dropWarning(std::string str);
};

#endif
