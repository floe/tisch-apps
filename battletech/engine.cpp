/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "engine.h"
#include "unitListMenu.h"
#include "infoMenu.h"
#include "warningMenu.h"
#include "damageResolutionMenu.h"
#include "victoryMenu.h"

engine::engine() 
{ 
	unitDeploymentPhase = false; movementPhase = false; attackPhase = false; unitIsMoving = false; round = 1; 
	activeWarning = false;
};

//roll 2D6
int engine::roll()
{
	int a = (int)(1 + ( rand() * ( 1.0 / ( RAND_MAX + 1.0 ) ) ) * 6);
	int b = (int)(1 + ( rand() * ( 1.0 / ( RAND_MAX + 1.0 ) ) ) * 6);

	return (a+b);
}

int engine::StringToInt(std::string stringValue)
{
    std::stringstream ssStream(stringValue);
    int iReturn;
    ssStream >> iReturn;

    return iReturn;
}

std::string engine::IntToString(int iValue)
{
    std::stringstream ssStream;
    ssStream << iValue;
    return ssStream.str();
} 

std::string engine::removeUnderscore(std::string str)
{
	std::replace(str.begin(),str.end(),'_', ' ');
	return str;
}

//Initialization of Players and Gamefield
void engine::initPlayers()
{
	for(int i = 0; i < number_of_players; i++)
	{
		players.push_back(new player());
		player_rolls.push_back(-1);
		player_order.push_back(-1);
		switch(i)
		{
		case 0: players[i]->mycolor.push_back(1); players[i]->mycolor.push_back(0); players[i]->mycolor.push_back(0); break;
		case 1: players[i]->mycolor.push_back(0); players[i]->mycolor.push_back(1); players[i]->mycolor.push_back(0); break;
		case 2: players[i]->mycolor.push_back(1); players[i]->mycolor.push_back(0.5); players[i]->mycolor.push_back(0); break;
		case 3: players[i]->mycolor.push_back(1); players[i]->mycolor.push_back(1); players[i]->mycolor.push_back(0); break;
		case 4: players[i]->mycolor.push_back(1); players[i]->mycolor.push_back(0); players[i]->mycolor.push_back(1); break;
		case 5: players[i]->mycolor.push_back(0); players[i]->mycolor.push_back(1); players[i]->mycolor.push_back(1); break;
		case 6: players[i]->mycolor.push_back(1); players[i]->mycolor.push_back(1); players[i]->mycolor.push_back(1); break;
		case 7: players[i]->mycolor.push_back(0.5); players[i]->mycolor.push_back(0); players[i]->mycolor.push_back(0); break;
		}
		players[i]->mycolor.push_back(1);
	}
	field = new gamefield(800,600,0,0);
}

//add unit "unitnumber" to player "playernumber"
void engine::addUnitToPlayer(int playernumber, int unitnumber)
{
	switch(unitnumber)
	{
	case 0:
		tex_storage.loadUnitTexture(0);
		players[playernumber]->addUnit(new mech("ENF-4R_Enforcer", 0, playernumber, tex_storage.getUnitTexture(0)));
		break;

	case 1:
		tex_storage.loadUnitTexture(1);
		players[playernumber]->addUnit(new mech("HBK-4G_Hunchback", 1, playernumber, tex_storage.getUnitTexture(1)));
		break;

	case 2:
		tex_storage.loadUnitTexture(2);
		players[playernumber]->addUnit(new mech("Cicada_CDA-3MA", 2, playernumber, tex_storage.getUnitTexture(2)));
		break;
		
	case 3:
		tex_storage.loadUnitTexture(3);
		players[playernumber]->addUnit(new mech("Hermes_II_HER-5SA", 3, playernumber, tex_storage.getUnitTexture(3)));
		break;
		
	case 4://TODO
		tex_storage.loadUnitTexture(4);
		players[playernumber]->addUnit(new mech("ENF-4R_Enforcer", 4, playernumber, tex_storage.getUnitTexture(4)));
		break;
		
	case 5://TODO
		tex_storage.loadUnitTexture(5);
		players[playernumber]->addUnit(new mech("ENF-4R_Enforcer", 5, playernumber, tex_storage.getUnitTexture(5)));
		break;
		
	case 6://TODO
		tex_storage.loadUnitTexture(6);
		players[playernumber]->addUnit(new mech("ENF-4R_Enforcer", 6, playernumber, tex_storage.getUnitTexture(6)));
		break;
		
	case 7://TODO
		tex_storage.loadUnitTexture(7);
		players[playernumber]->addUnit(new mech("ENF-4R_Enforcer", 7, playernumber, tex_storage.getUnitTexture(7)));
		break;
	}
}

//remove all units from all players
void engine::removeAllUnits()
{
	for(int i = 0; i < number_of_players; i++)
		players[i]->removeAllUnits();
}

void engine::startDeploymentPhase()
{
	unitDeploymentPhase = true;
	for(int i = 0; i < number_of_players; i++)
		player_rolls[i] = roll();
	while(rerollTies());
	setOrder();
	currentPlayer = 0;
	openUnitListMenu(player_order[currentPlayer]);
	field->deploymentLightening();
}

void engine::startMovementPhase()
{
	movementPhase = true;
	field->showallMovementsLeft();
	for(int i = 0; i < number_of_players; i++)
	{
		player_rolls[i] = roll();
		gameengine.players[i]->turns_left = gameengine.players[i]->units.size();
	}
	while(rerollTies());
	setOrder();
	currentPlayer = 0;
	while(players[player_order[currentPlayer]]->defeated)
	{
		currentPlayer++;
		currentPlayer %= number_of_players;
	} 
	openInfoMenu();
}

void engine::startAttackPhase()
{
	field->selectedUnit = 0;
	attackPhase = true;
	for(int i = 0; i < number_of_players; i++)
		gameengine.players[i]->turns_left = gameengine.players[i]->units.size();
	currentPlayer = 0;
	while(players[player_order[currentPlayer]]->defeated)
	{
		currentPlayer++;
		currentPlayer %= number_of_players;
	} 
	field->showallAttackersLeft();
}

void engine::startDamageResolutionPhase()
{
	dmgResMenu = new damageResolutionMenu(780,580);
	field->add(dmgResMenu);
}

//reroll ties, till all ties are cleared
bool engine::rerollTies()
{
	for(int i = 0; i < number_of_players; i++)
		for(int j = 0; j < number_of_players; j++)
			if(i != j && player_rolls[i] == player_rolls[j])
				{
					player_rolls[i] = roll();
					player_rolls[j] = roll();
					return true;
				}
	return false;
}

//set player-order for this round (depends on rolls)
void engine::setOrder()
{
	int help;
	for(int i = 0; i < number_of_players; i++)
	{
		help = 0;
		for(int j = 0; j < number_of_players; j++)
			if(j != i && player_rolls[j] < player_rolls[i])
				help++;
		player_order[help] = i;
	}
}

//open the unitListMenu for player "playernumber"
void engine::openUnitListMenu(int playernumber)
{
	unitList = new unitListMenu(200,200,playernumber);
	field->add(unitList);
}

//starts next turn in Deployment Phase
void engine::nextPlayerDeployment()
{
	currentPlayer++;
	if(currentPlayer < number_of_players)
	{
		openUnitListMenu(player_order[currentPlayer]);
		field->deploymentLightening();
	}
	else
	{
		unitDeploymentPhase = false;
		startMovementPhase();
	}
}

void engine::openInfoMenu()
{
	if(movementPhase) field->infomenu = new infoMenu(200,140);
	else if(attackPhase) field->infomenu = new infoMenu(200,400);
	field->add(field->infomenu);
}

void engine::openVictoryMenu(int draw)
{
	this->vicMenu = new victoryMenu(250,100, draw);
	field->add(vicMenu);
}

//starts next turn in Movement Phase
void engine::nextPlayerMovement()
{
	field->selectedUnit->moved = true;
	players[player_order[currentPlayer]]->turns_left--;
	field->selectedUnit->num_of_hexes_moved = field->hexesMoved();
	unitIsMoving = false;
	field->selectedUnit = 0;
	field->playerHasRightToMoveUnit = false;
	field->resetallLookDirs();
	field->lightenall(false);
	field->showallMovementsLeft();

	int finishedMovementPhase = 0;
	do
	{
		currentPlayer++;
		currentPlayer %= number_of_players;
		finishedMovementPhase++;
	} while(players[player_order[currentPlayer]]->turns_left == 0 && finishedMovementPhase < number_of_players + 1
		|| players[player_order[currentPlayer]]->defeated);
	if(finishedMovementPhase >= number_of_players + 1)
	{
		movementPhase = false;
		startAttackPhase();
	}
	openInfoMenu();
	//field->refresh();
}

//starts next turn in Attack Phase
void engine::nextPlayerAttack()
{
	field->selectedUnit->attacked = true;
	players[player_order[currentPlayer]]->turns_left--;
	field->selectedUnit = 0;
	field->selectedTargetUnit = 0;
	field->showallAttackersLeft();

	int finishedAttackPhase = 0;
	do
	{
		currentPlayer++;
		currentPlayer %= number_of_players;
		finishedAttackPhase++;
	} while(players[player_order[currentPlayer]]->turns_left == 0 && finishedAttackPhase < number_of_players + 1
		|| players[player_order[currentPlayer]]->defeated);
	if(finishedAttackPhase >= number_of_players + 1)
	{
		attackPhase = false;
		startDamageResolutionPhase();
		return;
	}
	openInfoMenu();
	//field->refresh();
}

void engine::calculate_modified_hit_number()
{
	modified_hit_number = 
				4 //base to-hit modifier
			  + field->get_range_modifier() //range modifier
			  + field->selectedUnit->attacker_movement_modifier //attacker-movement modifier
			  + field->get_target_movement_modifier() //target-movement modifier
			  + field->get_terrain_modifier(); //terrain modifier*/
	calculate_hit_probability();
}

void engine::calculate_hit_probability()
{
	switch(modified_hit_number)
	{
	case 2:		hit_probability = 100; return;
	case 3:		hit_probability = 97; return;
	case 4:		hit_probability = 92; return;
	case 5:		hit_probability = 83; return;
	case 6:		hit_probability = 72; return;
	case 7:		hit_probability = 58; return;
	case 8:		hit_probability = 42; return;
	case 9:		hit_probability = 28; return;
	case 10:	hit_probability = 17; return;
	case 11:	hit_probability = 8; return;
	case 12:	hit_probability = 3; return;
	}
	if(modified_hit_number < 2) hit_probability = 100;
	else hit_probability = 0;
}

int engine::get_hit_location()
{
	int help = roll(); //hit-roll
	if(help < modified_hit_number) return -1; //miss
	help = roll(); //hit_locataion-roll
	return gameengine.field->selectedTargetUnit->hit_location[help];
}

void engine::endRound()
{
	//remove destroyed units
	for(int i = 0; i < (signed)players.size(); i++)
	{
		for(int j = players[i]->units.size() - 1; j > -1; j--)
		{
			if(players[i]->units[j]->destroyed)
			{
				/*field->tile[players[i]->units[j]->mycoords[0]][players[i]->units[j]->mycoords[1]]->remove(
						field->tile[players[i]->units[j]->mycoords[0]][players[i]->units[j]->mycoords[1]]->unitTile);
				delete field->tile[players[i]->units[j]->mycoords[0]][players[i]->units[j]->mycoords[1]]->unitTile;*/
				field->tile[players[i]->units[j]->mycoords[0]][players[i]->units[j]->mycoords[1]]->revertDeployment();
				players[i]->units.erase( players[i]->units.begin() + j );
			}
		}
		if(players[i]->units.size() == 0) players[i]->defeated = true;
	}
	int not_defeated_players = check_victory();
	if(not_defeated_players == 0)
		openVictoryMenu(0);
	else if(not_defeated_players == 1)
		openVictoryMenu(1);
	else
		startNextRound();
}

void engine::startNextRound()
{
	this->round++;
	this->unitIsMoving = false;

	field->playerHasRightToMoveUnit = false;
	field->selectedUnit = 0;
	field->selectedTargetUnit = 0;

	for(int i = 0; i < (signed)gameengine.players.size(); i++)
	{
		for(int j = 0; j < (signed)gameengine.players[i]->units.size(); j++)
		{
			gameengine.players[i]->units[j]->attacked = false;
			gameengine.players[i]->units[j]->current_round_target = 0;
			gameengine.players[i]->units[j]->attacker_movement_modifier = 0;
			gameengine.players[i]->units[j]->moved = false;
			gameengine.players[i]->units[j]->num_of_hexes_moved = 0;
			for(int k = 0; k < (signed)gameengine.players[i]->units[j]->weapons.size(); k++)
			{
				gameengine.players[i]->units[j]->weapons[k]->current_round_hit_location = -1;
				gameengine.players[i]->units[j]->weapons[k]->fired = false;
			}
		}
	}

	//field->refresh();

	this->startMovementPhase();
}

int engine::check_victory()
{
	int help = 0;
	for(int i = 0; i < (signed)players.size(); i++)
		if(!players[i]->defeated) 
			help++;
	return help;
}

void engine::resetValuesForNewGame()
{
	players.clear();
	player_rolls.clear();
	player_order.clear();
	round = 1;
}

engine gameengine;
