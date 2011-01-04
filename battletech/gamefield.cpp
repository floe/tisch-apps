/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "gamefield.h"
#include "engine.h"
#include "unitListMenu.h"
#include "warningMenu.h"
#include "mainMenu.h"
#include <MasterContainer.h>

//defines for intersection tests
#define LEFT 1
#define STRAIGHT 0
#define RIGHT -1
//helperclass for intersection tests
class intersectionhelper
{
public:
	intersectionhelper(int _modifier, int _hexesIntersected, bool _edgeProblem)
	{ modifier = _modifier; hexesIntersected = _hexesIntersected; edgeProblem = _edgeProblem; solved = false; };

	int modifier, hexesIntersected;
	bool edgeProblem, solved;
};


gamefield::gamefield(int w, int h, int x, int y) : Container(w, h, x, y)
{
	playerHasRightToMoveUnit = false;
	this->mytex = 0;
	selectedUnit = 0;
	selectedTargetUnit = 0;
	tilewidth = 17.5;
	tileheight = 20* tilewidth / 17.5;

	float pointX = 0;
	float pointY = 0;
	xMax = w / (2 * tilewidth);
	yMax = h / (1.5 * tileheight);

	for(int j = 0; j < yMax; j++)
	{
		for(int i = 0; i < xMax; i++)
		{
			if(j%2 == 0)
				pointX = i * (2 * tilewidth) + tilewidth;
			else
				pointX = (i+1) * (2 * tilewidth);
			pointY = j * (1.5 * tileheight) + tileheight;
			tile[i][j] = new gametile(pointX-w/2, pointY-h/2, tilewidth, tileheight, getTileType(i,j),i,j);
			this->add(tile[i][j]);
		}
	}
	selectioncircle_blue = new selectioncircle(0, tilewidth);
	selectioncircle_red = new selectioncircle(1, tilewidth);
	this->add(selectioncircle_blue);
	this->add(selectioncircle_red);
}

//assign tile Type (clear field, low woods, high woods) to tile[i][j]
int gamefield::getTileType(int i, int j)
{
	int sum = 0;
	if(i == 0 && j == 0);
	else if(j == 0 && i != 0)
		sum = tile[i-1][j]->type;
	else if(j != 0 && i == 0 && j%2)
		sum = tile[0][j-1]->type + tile[1][j-1]->type;
	else if(j != 0 && i == 0 && !(j%2))
		sum = tile[0][j-1]->type;
	else if(!(j%2))
		sum = tile[i-1][j]->type + tile[i-1][j-1]->type + tile[i][j-1]->type;
	else if(j%2)
	{
		sum = tile[i-1][j]->type + tile[i][j-1]->type;
		if(i < xMax - 1)
			sum += tile[i+1][j-1]->type;
	}

	int rdm = rand()%100;
	switch(sum)// E = Empty, C = Clear Field, L = Low Woods, H = High Woods
	{
	case 0://EEE
		if(rdm < 50) return 1;
		else if(rdm < 85) return 4;
		else return 13;
	case 1://CEE
		if(rdm < 60) return 1;
		else return 4;
	case 2://CCE
		if(rdm < 40) return 1;
		else return 4;
	case 3://CCC
		if(rdm < 20) return 1;
		else return 4;
	case 4://LEE
		if(rdm < 40) return 1;
		else if (rdm < 80) return 4;
		else return 13;
	case 5://LCE
		if(rdm < 50) return 1;
		else return 4;
	case 6://LCC
		if(rdm < 50) return 1;
		else return 4;
	case 8://LLE
		if(rdm < 40) return 1;
		else if(rdm < 80) return 4;
		else return 13;
	case 9://LLC
		if(rdm < 60) return 1;
		else return 4;
	case 12://LLL
		if(rdm < 20) return 1;
		else if(rdm < 40) return 4;
		else return 13;
	case 13://HEE
		if(rdm < 70) return 4;
		else return 13;
	case 17://HLE
		if(rdm < 70) return 4;
		else return 13;
	case 18://HLC
		return 4;
	case 21://HLL
		if(rdm < 70) return 4;
		else return 13;
	case 26://HHE
		if(rdm < 60) return 4;
		else return 13;
	case 30://HHL
		if(rdm < 70) return 4;
		else return 13;
	case 39://HHH
		if(rdm < 80) return 4;
		else return 13;
	default:
		return 1; //never reached
	}
}

//lightens or darkens all tiles
void gamefield::lightenall(bool light)
{
	for(int i = 0; i < xMax; i++)
		for(int j = 0; j < yMax; j++)
			tile[i][j]->lightened = light;
}

//sets all looking directions (red triangles) to false
void gamefield::resetallLookDirs()
{
	for(int i = 0; i < xMax; i++)
		for(int j = 0; j < yMax; j++)
			for(int k = 0; k < 6; k++)
				tile[i][j]->lookdir[k] = false;
}

//shows possible movements for all units
void gamefield::showallLookDirs()
{
	lightenall(true);
	for(int i = 0; i < (signed)gameengine.players.size(); i++)
		for(int j = 0; j < (signed)gameengine.players[i]->units.size(); j++)
			if(!gameengine.players[i]->units[j]->moved)
				starttraverse(gameengine.players[i]->units[j]->mycoords[0], gameengine.players[i]->units[j]->mycoords[1],
						/*TODO*/((mech*)gameengine.players[i]->units[j])->walkspeed, gameengine.players[i]->units[j]->facing,
							/*TODO running also*/ true);
}

//shows all units, that haven't moved this turn
void gamefield::showallMovementsLeft()
{
	lightenall(false);
	for(int i = 0; i < (signed)gameengine.players.size(); i++)
		for(int j = 0; j < (signed)gameengine.players[i]->units.size(); j++)
			if(!(gameengine.players[i]->units[j])->moved)
				tile[(gameengine.players[i]->units[j])->mycoords[0]][(gameengine.players[i]->units[j])->mycoords[1]]->lightened = true;
}

//shows all units, that haven't attacked this turn
void gamefield::showallAttackersLeft()
{
	lightenall(false);
	for(int i = 0; i < (signed)gameengine.players.size(); i++)
		for(int j = 0; j < (signed)gameengine.players[i]->units.size(); j++)
			if(!(gameengine.players[i]->units[j])->attacked)
				tile[(gameengine.players[i]->units[j])->mycoords[0]][(gameengine.players[i]->units[j])->mycoords[1]]->lightened = true;
}

//traverse function to determine possible moves (starting point)
void gamefield::starttraverse(int i, int j, int range, int dir, bool walking)
{
	tile[i][j]->lightened = true;
	tile[i][j]->lookdir[dir] = true;
	if(range <= 0) return;
	if(range >= 1) {	tile[i][j]->lookdir[(dir+1)%6] = true;
						tile[i][j]->lookdir[(dir-1+6)%6] = true;	}
	if(range >= 2) {	tile[i][j]->lookdir[(dir+2)%6] = true;
						tile[i][j]->lookdir[(dir-2+6)%6] = true;	}
	if(range >= 3) tile[i][j]->lookdir[(dir+3)%6] = true;
	
	if(j%2 == 1)
	{
		for(int a = -2; a <= 3; a++)
		{
			switch((6+dir+a)%6)
			{
			case 0: gamefield::traverse(i+1,j+1,range - fabs((double)a) - 1, 0, walking); break;
			case 1: gamefield::traverse(i+1,j  ,range - fabs((double)a) - 1, 1, walking); break;
			case 2: gamefield::traverse(i+1,j-1,range - fabs((double)a) - 1, 2, walking); break;
			case 3: gamefield::traverse(i  ,j-1,range - fabs((double)a) - 1, 3, walking); break;
			case 4: gamefield::traverse(i-1,j  ,range - fabs((double)a) - 1, 4, walking); break;
			case 5: gamefield::traverse(i  ,j+1,range - fabs((double)a) - 1, 5, walking); break;
			}
		}
	}
	else
	{
		for(int a = -2; a <= 3; a++)
		{
			switch((6+dir+a)%6)
			{
			case 0: gamefield::traverse(i  ,j+1,range - fabs((double)a) - 1, 0, walking); break;
			case 1: gamefield::traverse(i+1,j  ,range - fabs((double)a) - 1, 1, walking); break;
			case 2: gamefield::traverse(i  ,j-1,range - fabs((double)a) - 1, 2, walking); break;
			case 3: gamefield::traverse(i-1,j-1,range - fabs((double)a) - 1, 3, walking); break;
			case 4: gamefield::traverse(i-1,j  ,range - fabs((double)a) - 1, 4, walking); break;
			case 5: gamefield::traverse(i-1,j+1,range - fabs((double)a) - 1, 5, walking); break;
			}
		}
	}
	if(!walking) return;
	if(j%2 == 1)
	{
		for(int a = -2; a <= 3; a++)
		{
			switch((6+dir+(3-a))%6)
			{
			case 0: gamefield::traverse(i+1,j+1,range - fabs((double)a) - 1, 3, walking); break;
			case 1: gamefield::traverse(i+1,j  ,range - fabs((double)a) - 1, 4, walking); break;
			case 2: gamefield::traverse(i+1,j-1,range - fabs((double)a) - 1, 5, walking); break;
			case 3: gamefield::traverse(i  ,j-1,range - fabs((double)a) - 1, 0, walking); break;
			case 4: gamefield::traverse(i-1,j  ,range - fabs((double)a) - 1, 1, walking); break;
			case 5: gamefield::traverse(i  ,j+1,range - fabs((double)a) - 1, 2, walking); break;
			}
		}
	}
	else
	{
		for(int a = -2; a <= 3; a++)
		{
			switch((6+dir+(3-a))%6)
			{
			case 0: gamefield::traverse(i  ,j+1,range - fabs((double)a) - 1, 3, walking); break;
			case 1: gamefield::traverse(i+1,j  ,range - fabs((double)a) - 1, 4, walking); break;
			case 2: gamefield::traverse(i  ,j-1,range - fabs((double)a) - 1, 5, walking); break;
			case 3: gamefield::traverse(i-1,j-1,range - fabs((double)a) - 1, 0, walking); break;
			case 4: gamefield::traverse(i-1,j  ,range - fabs((double)a) - 1, 1, walking); break;
			case 5: gamefield::traverse(i-1,j+1,range - fabs((double)a) - 1, 2, walking); break;
			}
		}
	}
}

//traverse function to determine possible moves
void gamefield::traverse(int i, int j, int range, int dir, bool walking)
{
	if(i == xMax || j == yMax || i < 0 || j < 0) return;
	if(range < 0) return;
	range = range - tile[i][j]->movemodifier;
	tile[i][j]->lightened = true;
	tile[i][j]->lookdir[dir] = true;
	if(range <= 0)	return;
	if(range >= 1) {	tile[i][j]->lookdir[(dir+1)%6] = true;
						tile[i][j]->lookdir[(dir-1+6)%6] = true;	}
	if(range >= 2) {	tile[i][j]->lookdir[(dir+2)%6] = true;
						tile[i][j]->lookdir[(dir-2+6)%6] = true;	}
	if(range >= 3) tile[i][j]->lookdir[(dir+3)%6] = true;
	
	if(j%2 == 1)
	{
		for(int a = -2; a <= 3; a++)
		{
			switch((6+dir+a)%6)
			{
			case 0: gamefield::traverse(i+1,j+1,range - fabs((double)a) - 1, 0, walking); break;
			case 1: gamefield::traverse(i+1,j  ,range - fabs((double)a) - 1, 1, walking); break;
			case 2: gamefield::traverse(i+1,j-1,range - fabs((double)a) - 1, 2, walking); break;
			case 3: gamefield::traverse(i  ,j-1,range - fabs((double)a) - 1, 3, walking); break;
			case 4: gamefield::traverse(i-1,j  ,range - fabs((double)a) - 1, 4, walking); break;
			case 5: gamefield::traverse(i  ,j+1,range - fabs((double)a) - 1, 5, walking); break;
			}
		}
	}
	else
	{
		for(int a = -2; a <= 3; a++)
		{
			switch((6+dir+a)%6)
			{
			case 0: gamefield::traverse(i  ,j+1,range - fabs((double)a) - 1, 0, walking); break;
			case 1: gamefield::traverse(i+1,j  ,range - fabs((double)a) - 1, 1, walking); break;
			case 2: gamefield::traverse(i  ,j-1,range - fabs((double)a) - 1, 2, walking); break;
			case 3: gamefield::traverse(i-1,j-1,range - fabs((double)a) - 1, 3, walking); break;
			case 4: gamefield::traverse(i-1,j  ,range - fabs((double)a) - 1, 4, walking); break;
			case 5: gamefield::traverse(i-1,j+1,range - fabs((double)a) - 1, 5, walking); break;
			}
		}
	}
	if(!walking) return;
	if(j%2 == 1)
	{
		for(int a = -2; a <= 3; a++)
		{
			switch((6+dir+(3-a))%6)
			{
			case 0: gamefield::traverse(i+1,j+1,range - fabs((double)a) - 1, 3, walking); break;
			case 1: gamefield::traverse(i+1,j  ,range - fabs((double)a) - 1, 4, walking); break;
			case 2: gamefield::traverse(i+1,j-1,range - fabs((double)a) - 1, 5, walking); break;
			case 3: gamefield::traverse(i  ,j-1,range - fabs((double)a) - 1, 0, walking); break;
			case 4: gamefield::traverse(i-1,j  ,range - fabs((double)a) - 1, 1, walking); break;
			case 5: gamefield::traverse(i  ,j+1,range - fabs((double)a) - 1, 2, walking); break;
			}
		}
	}
	else
	{
		for(int a = -2; a <= 3; a++)
		{
			switch((6+dir+(3-a))%6)
			{
			case 0: gamefield::traverse(i  ,j+1,range - fabs((double)a) - 1, 3, walking); break;
			case 1: gamefield::traverse(i+1,j  ,range - fabs((double)a) - 1, 4, walking); break;
			case 2: gamefield::traverse(i  ,j-1,range - fabs((double)a) - 1, 5, walking); break;
			case 3: gamefield::traverse(i-1,j-1,range - fabs((double)a) - 1, 0, walking); break;
			case 4: gamefield::traverse(i-1,j  ,range - fabs((double)a) - 1, 1, walking); break;
			case 5: gamefield::traverse(i-1,j+1,range - fabs((double)a) - 1, 2, walking); break;
			}
		}
	}
}

//selected unit is moved
void gamefield::moveUnit(int i, int j)
{
	selectedUnit->moved = true;
	int old_i = selectedUnit->mycoords[0];
	int old_j = selectedUnit->mycoords[1];
	tile[old_i][old_j]->remove(tile[old_i][old_j]->unitTile);
	tile[old_i][old_j]->occupied = false;
	tile[old_i][old_j]->Unit = 0;

	selectedUnit->mycoords[0] = i;
	selectedUnit->mycoords[1] = j;
	tile[i][j]->Unit = selectedUnit;
	tile[i][j]->occupied = true;
	tile[i][j]->unitTile = tile[old_i][old_j]->unitTile;
	tile[i][j]->add(tile[i][j]->unitTile);
	if(!tile[i][j]->lookdir[selectedUnit->facing])
		for(int k = 0; k < 6; k++)
			if(tile[i][j]->lookdir[k])
			{
				selectedUnit->facing = k;
				tile[i][j]->unitTile->setFacing(k);
				break;
			}
}

//calculates the number of hexes traversed by the unit moved this turn
int gamefield::hexesMoved(int start_i, int start_j, int finish_i, int finish_j)
{
	int number = 0;
	if(start_i == -1) //if called with default values, calculate number of hexes moved this turn
	{
		start_i = selectedUnit_old_i;
		start_j = selectedUnit_old_j;
		finish_i = selectedUnit->mycoords[0];
		finish_j = selectedUnit->mycoords[1];
	}
	//same row
	if(start_j == finish_j) number = fabs((double)start_i - finish_i);
	//different row
	else if(!(start_j%2))//start_j is even
	{
		if(finish_i < start_i - (int)(fabs((double)start_j - finish_j)/2+0.5))
			number = fabs((double)start_j - finish_j) + (start_i - (int)(fabs((double)start_j - finish_j)/2+0.5)) - finish_i;
		else if(finish_i > start_i + (int)(fabs((double)start_j - finish_j)/2))
			number = fabs((double)start_j - finish_j) + finish_i - (start_i + (int)(fabs((double)start_j - finish_j)/2));
		else	
			number = fabs((double)start_j - finish_j);
	}
	else //start_j is odd
	{
		if(finish_i < start_i - (int)(fabs((double)start_j - finish_j)/2))
			number = fabs((double)start_j - finish_j) + (start_i - (int)(fabs((double)start_j - finish_j)/2)) - finish_i;
		else if(finish_i > start_i + (int)(fabs((double)start_j - finish_j)/2+0.5))
			number = fabs((double)start_j - finish_j) + finish_i - (start_i + (int)(fabs((double)start_j - finish_j)/2+0.5));
		else	
			number = fabs((double)start_j - finish_j);
	}
	return number;
}

//warning message with text "str" and "ok" button is opened
void gamefield::dropWarning(std::string str)
{
	gameengine.activeWarning = true;
	gameengine.warnMenu = new warningMenu(600,100);
	gameengine.warnMenu->setText(str);
	add(gameengine.warnMenu);
}

//lighten fields for deployment
void gamefield::deploymentLightening()
{
	if(gameengine.unitList->playernumber % 2 == 0)
		for(int i = 0; i < xMax; i++)
		{
			tile[i][0]->lightened = true;
			tile[i][1]->lightened = true;
			tile[i][2]->lightened = true;
			tile[i][yMax - 1]->lightened = false;
			tile[i][yMax - 2]->lightened = false;
			tile[i][yMax - 3]->lightened = false;
		}
	else
		for(int i = 0; i < xMax; i++)
		{
			tile[i][0]->lightened = false;
			tile[i][1]->lightened = false;
			tile[i][2]->lightened = false;
			tile[i][yMax - 1]->lightened = true;
			tile[i][yMax - 2]->lightened = true;
			tile[i][yMax - 3]->lightened = true;
		}
}

//Line of Sight check for weapon attacks
bool gamefield::checkLOS(int attacker_i, int attacker_j, int range)
{
	lightenall(false);
	int target_i, target_j;
	std::vector<intersectionhelper*> intersectionList;
	bool unit_in_LOS = false;
	
	//for all units of all players
	for(int u = 0; u < (signed)gameengine.players.size(); u++)
	{
		for(int v = 0; v < (signed)gameengine.players[u]->units.size(); v++)
		{
			target_i = gameengine.players[u]->units[v]->mycoords[0];
			target_j = gameengine.players[u]->units[v]->mycoords[1];
			if(attacker_i == target_i && attacker_j == target_j) //selected unit
				continue;
			
		//rangecheck
			if(hexesMoved(attacker_i, attacker_j, target_i, target_j) > range)
				continue;

		//anglecheck
			int lookdirX = 0.5 * (tile[attacker_i][attacker_j]->points[tile[attacker_i][attacker_j]->Unit->facing][0] 
									+ tile[attacker_i][attacker_j]->points[(tile[attacker_i][attacker_j]->Unit->facing+1)%6][0]);
			int lookdirY = 0.5 * (tile[attacker_i][attacker_j]->points[tile[attacker_i][attacker_j]->Unit->facing][1] 
									+ tile[attacker_i][attacker_j]->points[(tile[attacker_i][attacker_j]->Unit->facing+1)%6][1]);
			int pt1 = lookdirX - tile[attacker_i][attacker_j]->centerX;
			int pt2 = lookdirY - tile[attacker_i][attacker_j]->centerY;
			int pt3 = tile[target_i][target_j]->centerX - tile[attacker_i][attacker_j]->centerX;
			int pt4 = tile[target_i][target_j]->centerY - tile[attacker_i][attacker_j]->centerY;
			double angle = ( (pt1 * pt3) + (pt2 * pt4) )/((sqrt((double)pt1*pt1 + pt2*pt2)) * (sqrt((double)pt3*pt3 + pt4*pt4)) );
			angle = acos(angle) * 180 / M_PI;
			if(fabs(angle) > 63) //error-tolerance: 3
				continue;

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//intersectioncheck
			int i = attacker_i; int j = attacker_j; int sign_i = 1; int sign_j = 1;
			if(attacker_i > target_i) sign_i = -1;
			if(attacker_j > target_j) sign_j = -1;
			
			if(attacker_i == target_i && attacker_j%2 == target_j%2 && attacker_i != 0 && attacker_i != xMax)
			{
				if(attacker_j%2) { i = attacker_i + 1; sign_i = -1; }
				else { i = attacker_i - 1; sign_i = 1; }
			}
			if(attacker_j == target_j) //same row, so all tiles of this row between attacker_i and target_i are intersected
			{
				i += sign_i;
				for(; i != target_i; i += sign_i)
					intersectionList.push_back(new intersectionhelper(tile[i][j]->movemodifier, 
												hexesMoved(attacker_i, attacker_j, i, j), false));
			}
			else
			{
				//for all tiles between attacker_i/j and target_i/j: checkinterseciton
				j += sign_j; 
				for(; i != target_i + sign_i; i += sign_i)
				{
					for(; j != target_j + sign_j; j += sign_j)
					{
						if(i == target_i && j == target_j) continue; //target tile not considered in LOS calculation
						if(i == attacker_i && j == attacker_j) continue; //attacker tile not considered in LOS calculation

						switch(hexintersectsline(i,j,tile[attacker_i][attacker_j]->centerX, tile[attacker_i][attacker_j]->centerY,
												tile[target_i][target_j]->centerX, tile[target_i][target_j]->centerY))
						{
						case 0: //no intersection
							break;
						case 1: //exclusive intersection
							intersectionList.push_back(new intersectionhelper(tile[i][j]->movemodifier, 
														hexesMoved(attacker_i, attacker_j, i, j), false));
							break;
						case 2: //shared intersection
							intersectionList.push_back(new intersectionhelper(tile[i][j]->movemodifier, 
													hexesMoved(attacker_i, attacker_j, i, j), true));
							break;
						}
					}
					j = attacker_j;
				}
			}
			if(intersectionList.empty()) //Unit is direct neighbour
			{	
				tile[target_i][target_j]->lightened = true;
				unit_in_LOS = true;
				continue; //end intersectioncheck
			}

	//------------------------------------------------------------------------------------------------------------------
	//solveEdgeProblem
			for(int a = 0; a < (signed)intersectionList.size(); a++)
				if(intersectionList[a]->edgeProblem && !intersectionList[a]->solved)
					for(int b = 0; b < (signed)intersectionList.size(); b++)
						if(intersectionList[a]->hexesIntersected == intersectionList[b]->hexesIntersected && a != b)
						{
							intersectionList[a]->solved = true;
							intersectionList[b]->solved = true;
							if(intersectionList[a]->modifier > intersectionList[b]->modifier)
								intersectionList[a]->edgeProblem = false;
							else
								intersectionList[b]->edgeProblem = false;
						}
			for(int a = intersectionList.size() - 1; a > -1; a--)
				if(intersectionList[a]->edgeProblem)
					intersectionList.erase(intersectionList.begin() + a);
	//------------------------------------------------------------------------------------------------------------------

			int sum = 0;
			for(int a = 0; a < (signed)intersectionList.size(); a++)
				sum += intersectionList[a]->modifier;
			if(sum < 3) //LOS not blocked 
			{	
				tile[target_i][target_j]->lightened = true;
				unit_in_LOS = true;
				gameengine.players[u]->units[v]->terrain_modifier = sum;
			}
			intersectionList.clear();
//end of intersectioncheck
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
		}
	}
	return unit_in_LOS;
}

//http://www-cs-students.stanford.edu/~amitp/Articles/HexLOS.html
int gamefield::hexintersectsline(int i, int j, double x0, double y0, double x1, double y1) 
{
    int side1, help;
	bool straight = false;
    side1 = turns(x0, y0, x1, y1, tile[i][j]->points[0][0], tile[i][j]->points[0][1]);
	if (side1 == STRAIGHT) straight = true;
    for (int k = 1; k < 6; k++) 
	{
        help = turns(x0, y0, x1, y1, tile[i][j]->points[k][0], tile[i][j]->points[k][1]);
        if (help == STRAIGHT) straight = true;
		if (help != side1 && help != STRAIGHT) return 1;
    }
	if(straight) return 2;
    return 0;
}

//http://www-cs-students.stanford.edu/~amitp/Articles/HexLOS.html
int gamefield::turns(double x0, double y0, double x1, double y1, double x2, double y2) 
{
    double cross;
    cross = (x1-x0)*(y2-y0) - (x2-x0)*(y1-y0);
    return((cross>0.0) ? LEFT : ((cross==0.0) ? STRAIGHT : RIGHT));
}

int gamefield::get_range_modifier()
{
	int help = hexesMoved(selectedUnit->mycoords[0], selectedUnit->mycoords[1], 
							selectedTargetUnit->mycoords[0], selectedTargetUnit->mycoords[1]);
	if(help <= selectedWeapon->range_sht)
		return selectedUnit->to_hit_modifier_sht;
	else if(help <= selectedWeapon->range_med)
		return selectedUnit->to_hit_modifier_med;
	else if(help <= selectedWeapon->range_lng)
		return selectedUnit->to_hit_modifier_lng;
	else return 1000;
}

int gamefield::get_target_movement_modifier()
{
	int x = selectedTargetUnit->num_of_hexes_moved;
	return x < 3 ? 0 : x < 5 ? 1 : x < 7 ? 2 : x < 10 ? 3 : x < 18 ? 4 : x < 25 ? 5 : x > 24 ? 6  : x; 
}

int gamefield::get_terrain_modifier()
{
	 return tile[selectedTargetUnit->mycoords[0]][selectedTargetUnit->mycoords[1]]->movemodifier + selectedTargetUnit->terrain_modifier;
}

void gamefield::startNewGame()
{
	((Container*)this->parent)->add(new mainMenu(300,400,10,10,0.0, 0,0x07));
	gameengine.resetValuesForNewGame();
}

void gamefield::refresh()
{
	((MasterContainer*)this->parent)->signOff();
	update();
}