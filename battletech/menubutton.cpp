/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "menubutton.h"
#include "mainMenu.h"
#include "unitDataContainer.h"
#include "unitSelectionMenu.h"
#include "playerMenu.h"
#include "unitListMenu.h"
#include "warningMenu.h"
#include "infoMenu.h"
#include "engine.h"
#include "player.h"
#include <BlobID.h>
#include <BlobPos.h>
#include <BlobCount.h>

menubutton::menubutton(int w, int h, int buttontype, int x, int y, double angle, RGBATexture* tex): 
	Button(w,h,x,y,angle,tex),
	type(buttontype)
{	
	if(tex == 0)
		mytex = tex_storage.getbutton();
	
/*	Gesture tap( "tap", GESTURE_FLAGS_STICKY || GESTURE_FLAGS_ONESHOT );
	tap.push_back( new BlobID(1<<INPUT_TYPE_FINGER) );
	tap.push_back( new BlobPos(1<<INPUT_TYPE_FINGER) );
	region.gestures.push_back( tap );*/
}

void menubutton::setType(int num)
{
	type = num;
}

//action depends on type
void menubutton::tap(Vector pos, int id)
{
	if(gameengine.activeWarning && type != 19)
		return;
	
	switch(type)
	{
//Exit
	case 0: exit(0); break;

// increase Number of Players
	case 1: 
		if(((playerMenu*)this->parent)->num_of_players < 8) ((playerMenu*)this->parent)->num_of_players++;
		((playerMenu*)this->parent)->changeNumber();
		break;

// decrease Number of Players
	case 2: 
		if(((playerMenu*)this->parent)->num_of_players > 2 ) ((playerMenu*)this->parent)->num_of_players--;
		((playerMenu*)this->parent)->changeNumber();
		break;

//Start Game
	case 3:
		((mainMenu*)this->parent)->openPlayerMenu();
		delete this->parent;
		break;

//playerMenu OK-Button
	case 7:
		gameengine.number_of_players = ((playerMenu*)this->parent)->num_of_players;
		gameengine.initPlayers();
		((playerMenu*)this->parent)->openUnitSelectionMenu();
		((playerMenu*)this->parent)->openUnitListMenu();
		delete this->parent;
		break;

//playerMenu Cancel-Button
	case 8:
		((playerMenu*)this->parent)->openMainMenu();
		delete this->parent;
		break;

//Close Parent Window
	case 9:
		delete ((Container*)this->parent);
		break;

//UnitSelectionMenu Next-Button
	case 10:
		((unitSelectionMenu*)this->parent)->openUnitSelectionMenu(((unitSelectionMenu*)this->parent)->number + 1);
		delete this->parent;
		break;

//UnitSelectionMenu Back-Button
	case 11:
		if(((unitSelectionMenu*)this->parent)->number > 0)
			((unitSelectionMenu*)this->parent)->openUnitSelectionMenu(((unitSelectionMenu*)this->parent)->number - 1);
		else
			((unitSelectionMenu*)this->parent)->openPlayerMenu();
		delete this->parent;
		break;

//UnitSelectionMenu Add-Button
	case 12:
		((unitSelectionMenu*)this->parent)->addUnits();
		break;

//UnitSelectionMenu Start-Button
	case 13:
		for(int i = 0; i < gameengine.number_of_players; i++)
			if(gameengine.players[i]->units.empty())
			{	
				((menu*)this->parent)->dropWarning("You have to add at least one Unit for each Player!");
				return;
			}
		((unitSelectionMenu*)this->parent)->startDeploymentPhase();
		gameengine.startDeploymentPhase();
		delete this->parent;
		break;

//UnitListMenu Remove-Button
	case 14:
		((unitListMenu*)this->parent)->removeUnit();
		break;

//UnitListMenu Done-Button
	case 15:
		//check if all units of current player are deployed
		for(int i = 0; i < (signed)gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->units.size(); i++)
			if(!gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->units[i]->deployed)
			{
				((menu*)this->parent)->dropWarning("You have to deploy all your units!");
				return;
			}
		gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->mypos[0] = this->parent->x;
		gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->mypos[1] = this->parent->y;
		gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->myangle = this->parent->angle;
		gameengine.nextPlayerDeployment();
		delete this->parent;
		break;

//UnitListMenu Pick-Up-Button
	case 16:
		((unitListMenu*)this->parent)->revertDeployment();
		break;

//WarningMenu OK-Button
	case 19:
		gameengine.activeWarning = false;
		delete this->parent;
		break;

//InfoMenu Done-Button (Movement Phase)
	case 20:
		if(gameengine.field->selectedUnit == 0 || !gameengine.field->playerHasRightToMoveUnit)
		{
			((menu*)this->parent)->dropWarning("You have to select one of your units, which hasn't moved this round!");
			break;
		}
		gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->mypos[0] = this->parent->x;
		gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->mypos[1] = this->parent->y;
		gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->myangle = this->parent->angle;
		gameengine.nextPlayerMovement();
		this->parent->lower();
		delete this->parent;
		break;

//InfoMenu Done-Button (Attack Phase)
	case 21:
		if(gameengine.field->selectedUnit == 0)
		{
			((menu*)this->parent)->dropWarning("You have to select one of your units, which hasn't attacked this round!");
			break;
		}
		gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->mypos[0] = this->parent->x;
		gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->mypos[1] = this->parent->y;
		gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->myangle = this->parent->angle;
		gameengine.nextPlayerAttack();
		this->parent->lower();
		delete this->parent;
		break;

//InfoMenu Show-All-Button (Movement Phase)
	case 22:
		if(gameengine.unitIsMoving) break;
		if(gameengine.field->selectedUnit != 0) gameengine.field->infomenu->resetAll();
		gameengine.field->resetallLookDirs();
		gameengine.field->showallMovementsLeft();
		gameengine.field->selectedUnit = 0;
		gameengine.field->playerHasRightToMoveUnit = false;
		break;

//InfoMenu Show-All-Button (Attack Phase)
	case 23:
		if(gameengine.field->infomenu->target_locked) 
		{
			((menu*)this->parent)->dropWarning("Cannot attack multiple targets!");
			break;
		}
		if(gameengine.field->selectedUnit != 0) gameengine.field->infomenu->resetAll();
		gameengine.field->showallAttackersLeft();
		gameengine.field->selectedUnit = 0;
		gameengine.field->selectedTargetUnit = 0;
		break;

//InfoMenu Fire-Button (Attack-Phase)
	case 24:
		((infoMenu*)this->parent)->fire_weapon();
		break;

//damageResolutionMenu Done-Button
	case 30:
		gameengine.endRound();
		this->parent->lower();
		delete this->parent;
		break;

//victoryMenu Play-Again Button
	case 35:
		gameengine.field->startNewGame();
		delete gameengine.field;
		break;

//show "ENF-4R Enforcer" Datasheet (selected Unit)
	case 40: ((unitSelectionMenu*)this->parent)->openUnitDataContainer(0); break;
//show "HBK-4G Hunchback" Datasheet (selected Unit)
	case 41: ((unitSelectionMenu*)this->parent)->openUnitDataContainer(1); break;
//show "Cicada CDA-3MA" Datasheet (selected Unit)
	case 42: ((unitSelectionMenu*)this->parent)->openUnitDataContainer(2); break;
//show "Hermes II HER-5SA" Datasheet (selected Unit)
	case 43: ((unitSelectionMenu*)this->parent)->openUnitDataContainer(3); break;
//show "Rommel Tank" Datasheet (selected Unit)
	case 44: ((unitSelectionMenu*)this->parent)->openUnitDataContainer(4); break;
//show "Rommel Tank (Guass Variant)" Datasheet (selected Unit)
	case 45: ((unitSelectionMenu*)this->parent)->openUnitDataContainer(5); break;
//show "Saladin Assault Hover Tank" Datasheet (selected Unit)
	case 46: ((unitSelectionMenu*)this->parent)->openUnitDataContainer(6); break;
//show "Saladin (Ultra Variant)" Datasheet (selected Unit)
	case 47: ((unitSelectionMenu*)this->parent)->openUnitDataContainer(7); break;

//show "ENF-4R Enforcer" Datasheet (targeted Unit)
	case 50: ((unitSelectionMenu*)this->parent)->openUnitDataContainer(8); break;
//show "HBK-4G Hunchback" Datasheet (targeted Unit)
	case 51: ((unitSelectionMenu*)this->parent)->openUnitDataContainer(9); break;
//show "Cicada CDA-3MA" Datasheet (targeted Unit)
	case 52: ((unitSelectionMenu*)this->parent)->openUnitDataContainer(10); break;
//show "Hermes II HER-5SA" Datasheet (targeted Unit)
	case 53: ((unitSelectionMenu*)this->parent)->openUnitDataContainer(11); break;
//show "Rommel Tank" Datasheet (targeted Unit)
	case 54: ((unitSelectionMenu*)this->parent)->openUnitDataContainer(12); break;
//show "Rommel Tank (Guass Variant)" Datasheet (targeted Unit)
	case 55: ((unitSelectionMenu*)this->parent)->openUnitDataContainer(13); break;
//show "Saladin Assault Hover Tank" Datasheet (targeted Unit)
	case 56: ((unitSelectionMenu*)this->parent)->openUnitDataContainer(14); break;
//show "Saladin (Ultra Variant)" Datasheet (targeted Unit)
	case 57: ((unitSelectionMenu*)this->parent)->openUnitDataContainer(15); break;

	default: ;
	}
}