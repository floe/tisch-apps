/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <tisch.h>
#include <Container.h>
#include "gametile.h"
#include "selectioncircle.h"

class infoMenu;

class gamefield : public Container
{
public:
	gamefield(int w, int h, int x = 0, int y = 0);

	int getTileType(int i, int j); //assign tile Type (clear field, low woods, high woods) to tile[i][j]
	void lightenall(bool light); //lightens or darkens all tiles
	void resetallLookDirs(); //sets all looking directions (red triangles) to false
	void showallLookDirs(); //shows possible movements for all units
	void showallMovementsLeft(); //shows all units, that haven't moved this turn
	void showallAttackersLeft(); //shows all units, that haven't attacked this turn
	void starttraverse(int i, int j, int range, int dir, bool walking); //traverse function to determine possible moves (starting point)
	void traverse(int i, int j, int range, int dir, bool walking); //traverse function to determine possible moves
	void moveUnit(int i, int j); //selected unit is moved
	int hexesMoved(int start_i = -1, int start_j = -1, int finish_i = -1, int finish_j = -1); //calculates the number traversed hexes between start and finish
	void dropWarning(std::string str); //warning message with text "str" and "ok" button is opened
	void deploymentLightening(); //lighten fields for deployment

	bool checkLOS(int attacker_i, int attacker_j, int range);
	int hexintersectsline(int i, int j, double x0, double y0, double x1, double y1);
	int turns(double x0, double y0, double x1, double y1, double x2, double y2); //returns if line A is LEFT, RIGHT, STRAIGHT of/on line B

	int get_range_modifier();
	int get_target_movement_modifier();
	int get_terrain_modifier();

	void startNewGame();

	void refresh();

	gametile* tile[50][50]; //array consists of pointer to each gametile
	float tilewidth, tileheight;
	int xMax, yMax; //number of gametiles in x and y direction
	unit* selectedUnit;
	unit* selectedTargetUnit;
	weapon* selectedWeapon;
	bool playerHasRightToMoveUnit;
	int selectedUnit_old_i, selectedUnit_old_j; //needed for hexesMoved() function
	infoMenu* infomenu;
	selectioncircle* selectioncircle_blue;
	selectioncircle* selectioncircle_red;
};

#endif

