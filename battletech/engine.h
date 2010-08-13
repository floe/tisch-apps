/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef ENGINE_H
#define ENGINE_H

#include <stdlib.h>
#include <vector>
#include <sstream>
#include "player.h"
#include "unit.h"
#include <algorithm>
#include <iostream>
#include <string>
#include "gamefield.h"

class unitListMenu;
class warningMenu;
class damageResolutionMenu;
class victoryMenu;

class engine
{
public:
	engine();
	
	int roll();	//roll 2D6
	int StringToInt(std::string stringValue);
	std::string IntToString(int iValue);
	std::string removeUnderscore(std::string str);
	void initPlayers(); //Initialization of Players and Gamefield
	void addUnitToPlayer(int playernumber, int unitnumber); //add unit "unitnumber" to player "playernumber"
	void removeAllUnits(); //remove all units from all players
	void startDeploymentPhase();
	void startMovementPhase();
	void startAttackPhase();
	void startDamageResolutionPhase();
	bool rerollTies(); //reroll ties, till all ties are cleared
	void setOrder(); //set player-order for current round (depends on rolls)
	void openUnitListMenu(int playernumber); //open the unitListMenu for player "playernumber"
	void nextPlayerDeployment(); //starts next turn in Deployment Phase
	void openInfoMenu();
	void openVictoryMenu(int draw);
	void nextPlayerMovement(); //starts next turn in Movement Phase
	void nextPlayerAttack(); //starts next turn in Attack Phase
	void calculate_modified_hit_number();
	void calculate_hit_probability();
	int get_hit_location(); //calculates hit location of selectedTargetUnit from selectedWeapon
	void endRound();
	void startNextRound();
	int check_victory();
	void resetValuesForNewGame();
	
	bool unitDeploymentPhase;
	bool movementPhase;
	bool attackPhase;
	bool unitIsMoving;
	int number_of_players; //number of players participating in the game
	std::vector<player*> players;
	std::vector<int> player_rolls; //vector for saving player rolls of current round
	std::vector<int> player_order; //vector for saving player order based on rolls for current round
	gamefield* field;
	unitListMenu* unitList;
	int currentPlayer; //use "player_order[currentPlayer]" to get the active player
	warningMenu* warnMenu;
	damageResolutionMenu* dmgResMenu;
	int modified_hit_number;
	int hit_probability;
	int round;
	victoryMenu* vicMenu;
	bool activeWarning;
};

extern engine gameengine;
#endif

