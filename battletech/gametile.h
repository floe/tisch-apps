/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef GAMETILE_H
#define GAMETILE_H

#include <tisch.h>
#include <widgets/Container.h>
#include <Texture.h>
#include "textures.h"
#include "unit.h"
#include "unittile.h"

class gametile : public Container
{
public:
	gametile(float x, float y, float w, float h, int _type, int xCoord, int yCoord);

	void draw();
//	void drawSelectionCircle(); //draws a selection circle around the unit on this field (if selected)
	void tap(Vector pos, int id); //action depends on current Phase, followed by other dependencies
	void action(Gesture *gesture); //selecting units with shadows
	void release(); //select unit in movement phase, by lifting a (real)mech in the air
	bool checkLookDirs(); //checks if at least 1 looking direction is true ("red triangle is shown") 
	void revertDeployment(); //removes the unit from this tile (in Deployment Phase)

	bool lookdir[6]; //looking directions ("red triangles")
	bool lightened;
	bool occupied; //true, if there is a unit on this tile
	int movemodifier; //important for movement, depends on type
	unit* Unit; //the unit of this tile
	int type; //the type of this tile: 1 = plains, 4 = low woods, 13 = high woods
	unittile* unitTile; //a tile for the unit deployed on this tile
	float points[6][2]; //Corners of the Hex-Tile
	float centerX, centerY; //Center of the Hex-Field

private:
	float width, height;
	double tw, th;
	RGBATexture* texture;
	int mycoords[2];
	int texangle;
};

#endif

