/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "unitSelectionMenu.h"
#include "unitDataContainer.h"
#include "playerMenu.h"
#include "unitListMenu.h"
#include "gamefield.h"
#include "warningMenu.h"

unitSelectionMenu::unitSelectionMenu(int w, int h, int _number, int x, int y, double angle, RGBATexture* tex, int mode) : 
	menu(w, h, x, y, angle, tex, mode),
	number(_number)
{
	mytex = tex_storage.getmenu();
	this->add(new Label("Player",100,25,0,h/2-25));
	Label* help = new Label("",40,25,70,h/2-25);
	help->set(gameengine.IntToString(number+1));
	this->add(help);

	unitlist.push_back(new Checkbox(20,20,-w/2+25,h/2-55,0,tex_storage.getCheckboxTexture()));
	this->add(unitlist[0]);
	this->add(new menubutton(250,25,40,-w/2+180,h/2-55));
	this->add(new Label("ENF-4R Enforcer",250,25,-w/2+175,h/2-55));
	
	unitlist.push_back(new Checkbox(20,20,-w/2+25,h/2-85,0,tex_storage.getCheckboxTexture()));
	this->add(unitlist[1]);
	this->add(new menubutton(250,25,41,-w/2+180,h/2-85));
	this->add(new Label("HBK-4G Hunchback",250,25,-w/2+175,h/2-85));

	unitlist.push_back(new Checkbox(20,20,-w/2+25,h/2-115,0,tex_storage.getCheckboxTexture()));
	this->add(unitlist[2]);
	this->add(new menubutton(250,25,42,-w/2+180,h/2-115));
	this->add(new Label("Cicada CDA-3MA",250,25,-w/2+175,h/2-115));

	unitlist.push_back(new Checkbox(20,20,-w/2+25,h/2-145,0,tex_storage.getCheckboxTexture()));
	this->add(unitlist[3]);
	this->add(new menubutton(250,25,43,-w/2+180,h/2-145));
	this->add(new Label("Hermes II HER-5SA",250,25,-w/2+175,h/2-145));

	/*
	TODO: tanks?
	unitlist.push_back(new Checkbox(20,20,-w/2+25,h/2-175,0,tex_storage.getCheckboxTexture()));
	this->add(unitlist[4]);
	this->add(new menubutton(250,25,44,-w/2+180,h/2-175));
	this->add(new Label("Rommel Tank",250,25,-w/2+175,h/2-175));

	unitlist.push_back(new Checkbox(20,20,-w/2+25,h/2-205,0,tex_storage.getCheckboxTexture()));
	this->add(unitlist[5]);
	this->add(new menubutton(250,25,45,-w/2+180,h/2-205));
	this->add(new Label("Rommel Tank (Guass Variant)",250,25,-w/2+175,h/2-205));

	unitlist.push_back(new Checkbox(20,20,-w/2+25,h/2-235,0,tex_storage.getCheckboxTexture()));
	this->add(unitlist[6]);
	this->add(new menubutton(250,25,46,-w/2+180,h/2-235));
	this->add(new Label("Saladin Assault Hover Tank",250,25,-w/2+175,h/2-235));

	unitlist.push_back(new Checkbox(20,20,-w/2+25,h/2-265,0,tex_storage.getCheckboxTexture()));
	this->add(unitlist[7]);
	this->add(new menubutton(250,25,47,-w/2+180,h/2-265));
	this->add(new Label("Saladin (Ultra Variant)",250,25,-w/2+175,h/2-265));*/

	if(number == gameengine.number_of_players - 1)
	{
		this->add(new menubutton(w/4, 30,13, w/4,-h/2 + 25));
		this->add(new Label("Start", w/6, 30, w/4,-h/2 + 25));
	}
	else
	{
		this->add(new menubutton(w/4, 30,10, w/4,-h/2 + 25));
		this->add(new Label("Next", w/6, 30, w/4,-h/2 + 25));
	}

	this->add(new menubutton(w/4, 30,11,-w/4,-h/2 + 25));
	this->add(new Label("Back", w/6, 30,-w/4,-h/2 + 25));
	
	this->add(new menubutton(w/6, 30,12,0,-h/2 + 25));
	this->add(new Label("Add", w/8, 30,0,-h/2 + 25));
}

void unitSelectionMenu::openUnitDataContainer(int num)
{
	if(num < 8) this->add(new unitDataContainer(600,400,num));
	else this->add(new unitDataContainer(600,400,num%8,true));
}

void unitSelectionMenu::openUnitSelectionMenu(int num)
{
	((Container*)this->parent)->add(new unitSelectionMenu(400,250,num,0,0,0.0, tex_storage.getmenu()));
}

void unitSelectionMenu::openPlayerMenu()
{
	((Container*)this->parent)->add(new playerMenu(300,100));
	((Container*)this->parent)->deleteWidgets<unitListMenu>();
	gameengine.removeAllUnits();
}

void unitSelectionMenu::addUnits()
{
	for(int i = 0; i < (signed)unitlist.size(); i++)
	{
		if(unitlist[i]->get())
		{
			gameengine.addUnitToPlayer(number,i);
		}
	}
}

void unitSelectionMenu::startDeploymentPhase()
{
	((Container*)this->parent)->deleteWidgets<unitListMenu>();
	((Container*)this->parent)->add(gameengine.field);
}
