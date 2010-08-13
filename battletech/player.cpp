/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "player.h"

player::player(){ defeated = false; myangle = 0; mypos.push_back(0); mypos.push_back(0); }

void player::addUnit(unit* u)
{
	units.push_back(u);
}

void player::removeUnit(unit* u)
{
	if(!units.empty())
	{
		for(int i = (signed)units.size() - 1; i > -1 ; i--)
		{
			if(units[i]->getname() == u->getname())
			{
				delete units[i];
				units.erase(units.begin()+i);
				break;
			}
		}
	}
}

void player::removeAllUnits()
{
	for(int i = (signed)units.size() - 1; i > -1 ; i--)
			delete units[i];
	units.clear();
}