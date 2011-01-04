/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

/*
direction- and point-order of one tile

     p0
     /\
   5/  \0
p5 /    \ p1
  |      |
 4|      |1
  |      |
p4 \    / p2
   3\  /2
     \/ 
     p3
*/

#include "gametile.h"
#include "engine.h"
#include "unitListMenu.h"
#include "infoMenu.h"
#include <BlobDim.h>
#include <BlobDelay.h>

gametile::gametile(float x, float y, float w, float h, int _type, int xCoord, int yCoord) : Container(w,h,x,y)
{
	occupied = false;
	centerX = x;
	centerY = y;
	width = w;
	height = h;
	lightened = false;
	lookdir[0] = false;
	lookdir[1] = false;
	lookdir[2] = false;
	lookdir[3] = false;
	lookdir[4] = false;
	lookdir[5] = false;
	type = _type;
	mycoords[0] = xCoord;
	mycoords[1] = yCoord;
	unitTile = new unittile(0,0,w,h);
	texangle = 0;

	switch(type)
	{
	case 1:
		texture = tex_storage.getplains(); movemodifier = 0; break;
	case 4:
		texture = tex_storage.getlowwoods(rand()%3); movemodifier = 1; break;
	case 13:
		texture = tex_storage.gethighwoods(rand()%2); movemodifier = 2; break;
	default:;
	}
	tw = texture->width();
	th = texture->height();

	points[0][0] = centerX;			points[0][1] = centerY + height;
	points[1][0] = centerX + width; points[1][1] = centerY + 0.5 * height;
	points[2][0] = centerX + width; points[2][1] = centerY - 0.5 * height;
	points[3][0] = centerX;			points[3][1] = centerY - height;
	points[4][0] = centerX - width; points[4][1] = centerY - 0.5 * height;
	points[5][0] = centerX - width; points[5][1] = centerY + 0.5 * height;

#if USESHADOW
	Gesture shadow( "shadow"/*, GESTURE_FLAGS_ONESHOT*/ );
	BlobDim* bdim = new BlobDim(1<<INPUT_TYPE_SHADOW);
	std::vector<Dimensions> d;
	Dimensions* dim = new Dimensions();
	dim->x1 = -1000; dim->x2 = -1000; dim->y1 = -1000; dim->y2 = -1000; dim->size = 0;
	d.push_back(*dim);
	Dimensions* dim2 = new Dimensions();
	dim2->x1 = 1000; dim2->x2 = 1000; dim2->y1 = 1000; dim2->y2 = 1000; dim2->size = 10000;
	d.push_back(*dim2);
	bdim->bounds( d );
	shadow.push_back( bdim );
	
	std::vector<int> b;
	b.push_back( 15 ); //set delay (in frames)
	b.push_back( 15 );
	BlobDelay* bdel = new BlobDelay(1<<INPUT_TYPE_SHADOW);
	bdel->bounds( b );
	shadow.push_back( bdel );
	
	region.gestures.push_back( shadow );
	region.flags( 1<<INPUT_TYPE_SHADOW | 1<<INPUT_TYPE_FINGER );		
#else
	region.flags( 1<<INPUT_TYPE_FINGER );
#endif
}

//action depends on current Phase, followed by other dependencies
void gametile::tap(Vector pos, int id)
{
	if(gameengine.unitDeploymentPhase)
	{
		if(lightened && !occupied)
		{
			for(int i = 0; i < (signed)gameengine.unitList->checkboxlist.size(); i++)
				if(gameengine.unitList->checkboxlist[i]->get() && !gameengine.players[gameengine.unitList->playernumber]->units[i]->deployed)
				{
					Unit = gameengine.players[gameengine.unitList->playernumber]->units[i];
					Unit->deployed = true;
					Unit->mycoords[0] = mycoords[0];
					Unit->mycoords[1] = mycoords[1];
					unitTile->setTexture(Unit->gettexture());
					unitTile->color(gameengine.players[gameengine.unitList->playernumber]->mycolor[0],
									gameengine.players[gameengine.unitList->playernumber]->mycolor[1],
									gameengine.players[gameengine.unitList->playernumber]->mycolor[2],
									gameengine.players[gameengine.unitList->playernumber]->mycolor[3]);
					this->add(unitTile);
					occupied = true;
					unitTile->setFacing(Unit->facing);
					break;
				}
		}
		else if(lightened && occupied)
		{
			Unit->facing++;
			Unit->facing %= 6;
			unitTile->setFacing(Unit->facing);
		}
	}
	else if(gameengine.movementPhase)
	{
		//select unit
		if(occupied && !gameengine.unitIsMoving && gameengine.field->selectedUnit != Unit)
		{
			if(Unit->moved)
			{
				((gamefield*)this->parent)->dropWarning("Unit already moved this round!");
				return;
			}
			gameengine.field->infomenu->displayUnitInfo(Unit); //show unit info
			gameengine.field->selectedUnit = Unit;
			gameengine.field->selectedUnit_old_i = mycoords[0];
			gameengine.field->selectedUnit_old_j = mycoords[1];
			gameengine.field->selectioncircle_blue->setCoordinates(centerX, centerY);

			if(Unit->getmyplayer() == gameengine.player_order[gameengine.currentPlayer])
				gameengine.field->playerHasRightToMoveUnit = true;
			else
				gameengine.field->playerHasRightToMoveUnit = false;
			//show only movementpossibilities for selected unit
			gameengine.field->resetallLookDirs();
			gameengine.field->starttraverse(mycoords[0],mycoords[1],/*TODO*/((mech*)Unit)->walkspeed,Unit->facing,/*TODO*/true);
		}
		//rotate unit (by clicking on it)
		else if(occupied && gameengine.field->selectedUnit == Unit)
		{
			if(!gameengine.field->playerHasRightToMoveUnit)
			{
				((gamefield*)this->parent)->dropWarning("You can only move your own units!"); 
				return;
			}
			gameengine.unitIsMoving = true;
			do {
				Unit->facing++;
				Unit->facing %= 6;
			} while(!lookdir[Unit->facing]);
			unitTile->setFacing(Unit->facing);
		}
		//move selected unit to this field
		else if(!occupied && gameengine.field->selectedUnit != 0)
		{
			if(!gameengine.field->playerHasRightToMoveUnit)
			{
				((gamefield*)this->parent)->dropWarning("You can only move your own units!"); return;
			}
			if(checkLookDirs())
			{
				gameengine.unitIsMoving = true;
				gameengine.field->moveUnit(mycoords[0],mycoords[1]);
				gameengine.field->selectioncircle_blue->setCoordinates(centerX, centerY);
			}
		}
	}
	else if(gameengine.attackPhase)
	{
		//select attacker
		if(occupied && gameengine.field->selectedUnit == 0 && lightened)
		{
			if(Unit->getmyplayer() != gameengine.player_order[gameengine.currentPlayer])
			{
				((gamefield*)this->parent)->dropWarning("Unit doesn't belong to you!");
				return;
			}
			if(Unit->attacked)
			{
				((gamefield*)this->parent)->dropWarning("Unit already attacked this round!");
				return;
			}
			gameengine.field->selectedUnit = Unit;
			gameengine.field->selectioncircle_blue->setCoordinates(centerX, centerY);
			gameengine.field->infomenu->displayUnitInfo(Unit); //also shows possible targets for longest range weapon
		}

		//select target
		else if(occupied && lightened)
		{
			if(gameengine.field->infomenu->target_locked) 
			{
				((gamefield*)this->parent)->dropWarning("Cannot attack multiple targets!");
				return;
			}
			if(Unit->getmyplayer() == gameengine.field->selectedUnit->getmyplayer())
			{
				((gamefield*)this->parent)->dropWarning("Can't shoot your own units!");
				return;
			}
			
			gameengine.field->selectedTargetUnit = Unit;
			gameengine.field->selectioncircle_red->setCoordinates(centerX, centerY);
			//TODO show health info of target?
			gameengine.field->infomenu->displayTargetInfo(Unit);
			
			gameengine.field->infomenu->set_hit_prob_text(); //shows info about probability of hit in infoMenu
		}
	}
}

#if USESHADOW
//selecting units with shadows
void gametile::action(Gesture *gesture)
{
	if (gesture->name() == "shadow")
	{
		//std::cout << "shadow input" << std::endl;
		this->tap(0,0);
	}
	else
		Tile::action(gesture);
}

void gametile::release()
{
	if(gameengine.movementPhase)
	{
		this->tap(0,0);
	}
}
#endif

//checks if at least 1 looking direction is true ("red triangle is shown") 
bool gametile::checkLookDirs()
{
	for(int i = 0; i < 6; i++)
		if(lookdir[i]) return true;
	return false;
}

//removes the unit from this tile (in Deployment Phase)
void gametile::revertDeployment()
{
	if(gameengine.unitDeploymentPhase) Unit->deployed = false;
	this->remove(unitTile);
	occupied = false;
}

void gametile::draw()
{
	glPushAttrib(GL_TEXTURE_BIT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	if(lightened)	glColor4f(1,1,1,1);
	else			glColor4f(0.5,0.5,0.5,1);
	texture->bind();
	glBegin(GL_POLYGON);
		glTexCoord2f(0,   0.25*th);  glVertex2f(points[4][0], points[4][1]);
		glTexCoord2f(0.5*tw, 0);     glVertex2f(points[3][0], points[3][1]);
		glTexCoord2f(tw,   0.25*th);  glVertex2f(points[2][0], points[2][1]);
		glTexCoord2f(tw,   0.75*th);  glVertex2f(points[1][0], points[1][1]);
		glTexCoord2f(0.5*tw, th);     glVertex2f(points[0][0], points[0][1]);
		glTexCoord2f(0,   0.75*th);  glVertex2f(points[5][0], points[5][1]);
	glEnd();
	texture->release();
	glPopAttrib();

//looking directions shown as red triangles
	glColor4f(1.0,0,0,0.3);
	if(lookdir[0])
	{
		glBegin(GL_TRIANGLES);
		glVertex2f(centerX, centerY);
		glVertex2f(points[0][0], points[0][1]);
		glVertex2f(points[1][0], points[1][1]);
		glEnd();
	}
	if(lookdir[1])
	{
		glBegin(GL_TRIANGLES);
		glVertex2f(centerX, centerY);
		glVertex2f(points[1][0], points[1][1]);
		glVertex2f(points[2][0], points[2][1]);
		glEnd();
	}
	if(lookdir[2])
	{
		glBegin(GL_TRIANGLES);
		glVertex2f(centerX, centerY);
		glVertex2f(points[2][0], points[2][1]);
		glVertex2f(points[3][0], points[3][1]);
		glEnd();
	}
	if(lookdir[3])
	{
		glBegin(GL_TRIANGLES);
		glVertex2f(centerX, centerY);
		glVertex2f(points[3][0], points[3][1]);
		glVertex2f(points[4][0], points[4][1]);
		glEnd();
	}
	if(lookdir[4])
	{
		glBegin(GL_TRIANGLES);
		glVertex2f(centerX, centerY);
		glVertex2f(points[4][0], points[4][1]);
		glVertex2f(points[5][0], points[5][1]);
		glEnd();
	}
	if(lookdir[5])
	{
		glBegin(GL_TRIANGLES);
		glVertex2f(centerX, centerY);
		glVertex2f(points[5][0], points[5][1]);
		glVertex2f(points[0][0], points[0][1]);
		glEnd();
	}

	if(occupied) 
	{
		enter();
		unitTile->draw();
		leave();
	}

	//draw selectioncircle if the unit on this field is selected
	//if(Unit != 0)
	//	drawSelectionCircle();
}

/*
void gametile::drawSelectionCircle()
{
	double texheight, texwidth;
	glColor4f(1,1,1,1);
	texangle += 5;
	texangle %= 360;
	if(gameengine.field->selectedUnit != 0 && gameengine.field->selectedUnit == Unit)
	{
		enter();
		glRotated(texangle, 0, 0, 1); //rotate the quad (and therefore the texture)
		
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_DST_COLOR,GL_ZERO);				// Blend Screen Color With Zero (Black)
		tex_storage.getSelectionTexture(0)->bind(); 			// Select Mask Texture
		texheight = tex_storage.getSelectionTexture(0)->height();
		texwidth = tex_storage.getSelectionTexture(0)->width();
		glBegin(GL_QUADS);					
			glTexCoord2f(0.0f, 0.0f); 			glVertex3f(-1.5f * width, -1.5f * width, 1.0f);
			glTexCoord2f(texwidth, 0.0f); 		glVertex3f( 1.5f * width, -1.5f * width, 1.0f);
			glTexCoord2f(texwidth, texheight); 	glVertex3f( 1.5f * width,  1.5f * width, 1.0f);
			glTexCoord2f(0.0f, texheight); 		glVertex3f(-1.5f * width,  1.5f * width, 1.0f);
		glEnd();
		tex_storage.getSelectionTexture(0)->release();

		glBlendFunc(GL_ONE, GL_ONE);					// Copy every nonblack pixel from Texture to Screen
		tex_storage.getSelectionTexture(1)->bind();			// Select Colored Texture
		texheight = tex_storage.getSelectionTexture(1)->height();
		texwidth = tex_storage.getSelectionTexture(1)->width();
		glBegin(GL_QUADS);						
			glTexCoord2f(0.0f, 0.0f); 			glVertex3f(-1.5f * width, -1.5f * width, 1.0f);
			glTexCoord2f(texwidth, 0.0f); 		glVertex3f( 1.5f * width, -1.5f * width, 1.0f);
			glTexCoord2f(texwidth, texheight); 	glVertex3f( 1.5f * width,  1.5f * width, 1.0f);
			glTexCoord2f(0.0f, texheight); 		glVertex3f(-1.5f * width,  1.5f * width, 1.0f);
		glEnd();
		tex_storage.getSelectionTexture(1)->release();

		glEnable(GL_DEPTH_TEST);
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); //default func
		leave();
	}

	else if(gameengine.field->selectedTargetUnit != 0 && gameengine.field->selectedTargetUnit == Unit)
	{
		enter();
		glRotated(texangle, 0, 0, 1); //rotate the quad (and therefore the texture)
		
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_DST_COLOR,GL_ZERO);				// Blend Screen Color With Zero (Black)
		tex_storage.getSelectionTexture(0)->bind(); 			// Select Mask Texture
		texheight = tex_storage.getSelectionTexture(0)->height();
		texwidth = tex_storage.getSelectionTexture(0)->width();
		glBegin(GL_QUADS);					
			glTexCoord2f(0.0f, 0.0f); 			glVertex3f(-1.5f * width, -1.5f * width, 1.0f);
			glTexCoord2f(texwidth, 0.0f); 		glVertex3f( 1.5f * width, -1.5f * width, 1.0f);
			glTexCoord2f(texwidth, texheight); 	glVertex3f( 1.5f * width,  1.5f * width, 1.0f);
			glTexCoord2f(0.0f, texheight); 		glVertex3f(-1.5f * width,  1.5f * width, 1.0f);
		glEnd();
		tex_storage.getSelectionTexture(0)->release();

		glBlendFunc(GL_ONE, GL_ONE);					// Copy every nonblack pixel from Texture to Screen
		tex_storage.getSelectionTexture(2)->bind();			// Select Colored Texture
		texheight = tex_storage.getSelectionTexture(1)->height();
		texwidth = tex_storage.getSelectionTexture(1)->width();
		glBegin(GL_QUADS);						
			glTexCoord2f(0.0f, 0.0f); 			glVertex3f(-1.5f * width, -1.5f * width, 1.0f);
			glTexCoord2f(texwidth, 0.0f); 		glVertex3f( 1.5f * width, -1.5f * width, 1.0f);
			glTexCoord2f(texwidth, texheight); 	glVertex3f( 1.5f * width,  1.5f * width, 1.0f);
			glTexCoord2f(0.0f, texheight); 		glVertex3f(-1.5f * width,  1.5f * width, 1.0f);
		glEnd();
		tex_storage.getSelectionTexture(2)->release();

		glEnable(GL_DEPTH_TEST);
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); //default func
		leave();
	}
}*/
