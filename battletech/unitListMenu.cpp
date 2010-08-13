/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "unitListMenu.h"
#include "unitDataContainer.h"
#include "playerMenu.h"

unitListMenu::unitListMenu(int w, int h, int _playernumber, int x, int y, double angle, RGBATexture* tex, int mode) : 
	menu(w, h, x, y, angle, tex, mode),
	playernumber(_playernumber)
{
	mytex = tex_storage.getmenu();
	lastSize = gameengine.players[playernumber]->units.size();
	if(!gameengine.unitDeploymentPhase)
	{
		switch(playernumber)
		{
		case 0:	this->x = -300;	this->y =  225; break;
		case 1:	this->x =  300;	this->y =  225; break;
		case 2:	this->x = -300;	this->y =   75; break;
		case 3:	this->x =  300;	this->y =   75; break;
		case 4:	this->x = -300;	this->y =  -75; break;
		case 5:	this->x =  300;	this->y =  -75; break;
		case 6:	this->x = -300;	this->y = -225; break;
		case 7:	this->x =  300;	this->y = -225; break;
		}
	}

	this->add(new Label("Player",100,20,-50,h/2-15));
	Label* help = new Label("",40,20,10,h/2-15);
	help->set(gameengine.IntToString(playernumber+1));
	help->color(gameengine.players[playernumber]->mycolor[0],
				gameengine.players[playernumber]->mycolor[1],
				gameengine.players[playernumber]->mycolor[2],
				gameengine.players[playernumber]->mycolor[3]);
	this->add(help);

	if(!gameengine.unitDeploymentPhase)
	{
		this->add(new menubutton(100,20,14,0,-h/2+15));
		this->add(new Label("Remove",80,20,0,-h/2+15));
	}
	else
	{
		this->add(new menubutton(80,20,16,-w/4,-h/2+15));
		this->add(new Label("Pick Up",60,20,-w/4,-h/2+15));

		this->add(new menubutton(80,20,15,w/4,-h/2+15));
		this->add(new Label("Done",60,20,w/4,-h/2+15));
	}
	updateUnitList();
}

void unitListMenu::openUnitDataContainer(int num)
{
	((Container*)this->parent)->add(new unitDataContainer(600,400,num));
}

void unitListMenu::removeUnit()
{
	for(int i = (signed)checkboxlist.size()-1; i > -1; i--)
	{
		if(checkboxlist[i]->get())
		{
			gameengine.players[playernumber]->removeUnit(gameengine.players[playernumber]->units[i]);
		}
	}
}

void unitListMenu::clearList()
{
	for(int i = 0; i < (signed)checkboxlist.size(); i++)
	{
		this->remove(checkboxlist[i]);
		delete checkboxlist[i];
	}
	for(int i = 0; i < (signed)unitLabel.size(); i++)
	{
		this->remove(unitLabel[i]);
		delete unitLabel[i];
	}
	for(int i = 0; i < (signed)unitButton.size(); i++)
	{
		this->remove(unitButton[i]);
		delete unitButton[i];
	}
	checkboxlist.clear();
	unitLabel.clear();
	unitButton.clear();
}

void unitListMenu::updateUnitList()
{
	clearList();

	int bh = 20;
	if(lastSize > 4)
		bh = 80/lastSize;
	for(int i = 0; i < lastSize; i++)
	{
		checkboxlist.push_back(new Checkbox(bh,bh,-w/2 + 15, h/2 - 40 - 1.1*bh*i,0,tex_storage.getCheckboxTexture()));
		this->add(checkboxlist[checkboxlist.size()-1]);
		unitButton.push_back(new menubutton(w-30,bh,40+gameengine.players[playernumber]->units[i]->getid(),12,h/2 - 40 - 1.1*bh*i));
		this->add(unitButton[unitButton.size()-1]);
		unitLabel.push_back(new Label("",w-30,bh-2,11,h/2 - 40 - 1.1*bh*i));
		unitLabel[unitLabel.size()-1]->set(gameengine.players[playernumber]->units[i]->getname());
		this->add(unitLabel[unitLabel.size()-1]);
	}
	if(gameengine.unitDeploymentPhase)
	{
		checkboxlist[0]->set(1);
		activeCheckbox = 0;
	}
}

void unitListMenu::draw()
{
	//Checkboxes as radiobuttons
	if(gameengine.unitDeploymentPhase)
	{
		for(int i = 0; i < (signed)checkboxlist.size(); i++)
			if(checkboxlist[i]->get() && activeCheckbox != i)
			{
				checkboxlist[activeCheckbox]->set(0);
				activeCheckbox = i;
				break;
			}
		checkboxlist[activeCheckbox]->set(1);
	}

	if(gameengine.players[playernumber]->units.size() != lastSize)
	{
		lastSize = gameengine.players[playernumber]->units.size();
		this->updateUnitList();
	}
	Container::draw();
}

void unitListMenu::revertDeployment()
{
	//remove selected Unit from corresponding tile
	if(gameengine.players[playernumber]->units[activeCheckbox]->deployed)
		gameengine.field->tile[gameengine.players[playernumber]->units[activeCheckbox]->mycoords[0]]
			[gameengine.players[playernumber]->units[activeCheckbox]->mycoords[1]]->revertDeployment();
}