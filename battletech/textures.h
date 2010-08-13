/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef TEXTURES_H
#define TEXTURES_H

#include <tisch.h>
#include <Texture.h>

class textures
{
public:
	textures();
	void init();
	void loadUnitTexture(int num);
	
	RGBATexture* getplains();
	RGBATexture* getbutton();
	RGBATexture* getmenu();
	RGBATexture* getBattletechTexture();
	RGBATexture* getlowwoods(int num);
	RGBATexture* gethighwoods(int num);
	RGBATexture* getUnitDataSheet(int num);
	RGBATexture* getUnitTexture(int num);
	RGBATexture* getContainerTexture();
	RGBATexture* getSliderTexture();
	RGBATexture* getCheckboxTexture();
	RGBATexture* getSelectionTexture(int num);


protected:
	RGBATexture* highwoods[2];
	RGBATexture* lowwoods[3];
	RGBATexture* plains;
	RGBATexture* button;
	RGBATexture* menu;
	RGBATexture* battletechTexture;
	RGBATexture* unitDataSheets[10];
	RGBATexture* unitTextures[8];
	RGBATexture* container;
	RGBATexture* slider;
	RGBATexture* checkbox;
	RGBATexture* selectionTextures[10];
};

extern textures tex_storage;

#endif

