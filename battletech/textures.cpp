/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "textures.h"

textures::textures()
{ }

void textures::init() {
	plains = new RGBATexture("textures/plains0.png");
	button = new RGBATexture("textures/Button.png");
	menu = new RGBATexture("textures/menu.png");
	battletechTexture = new RGBATexture("textures/Battletech.png");
	lowwoods[0] = new RGBATexture("textures/lowwoods0.png");
	lowwoods[1] = new RGBATexture("textures/lowwoods1.png");
	lowwoods[2] = new RGBATexture("textures/lowwoods2.png");
	highwoods[0] = new RGBATexture("textures/highwoods0.png");
	highwoods[1] = new RGBATexture("textures/highwoods1.png");
	unitDataSheets[0] = new RGBATexture("textures/ENF-4R_Enforcer.png");
	unitDataSheets[1] = new RGBATexture("textures/HBK-4G_Hunchback.png");
	unitDataSheets[2] = new RGBATexture("textures/Cicada_CDA-3MA.png");
	unitDataSheets[3] = new RGBATexture("textures/Hermes_II_HER-5SA.png");
	unitDataSheets[4] = new RGBATexture("textures/Rommel_Tank.png");
	unitDataSheets[5] = new RGBATexture("textures/Rommel_Tank_Guass_Variant.png");
	unitDataSheets[6] = new RGBATexture("textures/Saladin_Assault_Hover_Tank.png");
	unitDataSheets[7] = new RGBATexture("textures/Saladin_Ultra_Variant.png");
	container = new RGBATexture("textures/Container.png");
	slider = new RGBATexture("textures/Slider.png");
	checkbox = new RGBATexture("textures/Checkbox.png");
	selectionTextures[0] = new RGBATexture("textures/masked/Selectioncirclemask.png");
	selectionTextures[1] = new RGBATexture("textures/masked/Selectioncircleblue.png");
	selectionTextures[2] = new RGBATexture("textures/masked/Selectioncirclered.png");
}

void textures::loadUnitTexture(int num)
{
	if(unitTextures[num] != 0) return;

	switch(num)
	{
	case 0: unitTextures[0] = new RGBATexture("textures/units/Enforcer.png"); break;
	case 1: unitTextures[1] = new RGBATexture("textures/units/Hunchback.png"); break;
	case 2: unitTextures[2] = new RGBATexture("textures/units/Cicada.png"); break;
	case 3: unitTextures[3] = new RGBATexture("textures/units/Hermes.png"); break;
	case 4: unitTextures[4] = new RGBATexture("textures/units/Enforcer.png"); break;
	case 5: unitTextures[5] = new RGBATexture("textures/units/Enforcer.png"); break;
	case 6: unitTextures[6] = new RGBATexture("textures/units/Enforcer.png"); break;
	case 7: unitTextures[7] = new RGBATexture("textures/units/Enforcer.png"); break;
	}
}

RGBATexture* textures::getplains()						{	return plains;		}
RGBATexture* textures::getbutton()						{	return button;		}
RGBATexture* textures::getmenu()						{	return menu;		}
RGBATexture* textures::getBattletechTexture()			{	return battletechTexture;	}
RGBATexture* textures::getlowwoods(int num)				{	return lowwoods[num];	}
RGBATexture* textures::gethighwoods(int num)			{	return highwoods[num];	}
RGBATexture* textures::getUnitDataSheet(int num)		{	return unitDataSheets[num];	}
RGBATexture* textures::getUnitTexture(int num)			{	return unitTextures[num];	}
RGBATexture* textures::getContainerTexture()			{	return container;	}
RGBATexture* textures::getSliderTexture()				{	return slider;	}
RGBATexture* textures::getCheckboxTexture()				{	return checkbox;	}
RGBATexture* textures::getSelectionTexture(int num)		{	return selectionTextures[num];	}

textures tex_storage;
