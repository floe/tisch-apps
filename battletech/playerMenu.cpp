/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "playerMenu.h"
#include "menubutton.h"
#include "unitSelectionMenu.h"
#include "mainMenu.h"
#include "unitListMenu.h"

playerMenu::playerMenu(int w, int h, int x, int y, double angle, RGBATexture* tex, int mode): 
	menu(w,h,x,y,angle,tex,mode)
{	
	mytex = tex_storage.getmenu();
	num_of_players = 2;

	number = new Label("2", w/20, h/4, 0, h/30); 
	this->add(number);
	this->add(new menubutton(h/4,h/4,1,-0.3*w, h/30));
	this->add(new Label("+", h/8, h/4,-0.3*w, h/30));
	this->add(new menubutton(h/4,h/4,2,w*0.3, h/30));
	this->add(new Label("-", h/8, h/4, w*0.3, h/30));
	this->add(new Label("Select Number of Players",w-w/20,h/4,0,3*h/8));

	this->add(new menubutton(w/4,h/4,7,-w/7,-h/4-h/16));
	this->add(new Label("OK",w/8,h/4,-w/7,-h/4-h/16));
	this->add(new menubutton(w/4,h/4,8,w/7,-h/4-h/16));
	this->add(new Label("Cancel",w/4,h/4,w/7,-h/4-h/16));
	
}

void playerMenu::openUnitSelectionMenu()
{
	((Container*)this->parent)->add(new unitSelectionMenu(400,250,0,0,0));
}

void playerMenu::openUnitListMenu()
{
	for(int i = 0; i < num_of_players; i++)
		((Container*)this->parent)->add(new unitListMenu(200,150,i));
}

void playerMenu::openMainMenu()
{
	((Container*)this->parent)->add(new mainMenu(300,400,10,10,0.0));
}

void playerMenu::changeNumber()
{
	number->set(gameengine.IntToString(num_of_players));
}