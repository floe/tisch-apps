/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include "unit.h"

class player
{
public:
	player();
	void addUnit(unit* u);
	void removeUnit(unit* u);
	void removeAllUnits();

	std::vector<unit*> units;
	std::vector<double> mycolor;
	double myangle;
	std::vector<double> mypos;
	int turns_left;
	bool defeated;
};

#endif
