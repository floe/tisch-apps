/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "infoMenu.h"

infoMenu::infoMenu(int w, int h, int x, int y, double angle, RGBATexture* tex, int mode) : 
	menu(w, h, x, y, angle, tex, mode)
{
	mytex = tex_storage.getmenu();
	this->add(new Label("Player",w/2,20,0,h/2-40));
	Label* playernumber = new Label("", 30,20, w/4,h/2-40);
	playernumber->set(gameengine.IntToString(gameengine.player_order[gameengine.currentPlayer]+1));
	playernumber->color(gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->mycolor[0],
						gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->mycolor[1],
						gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->mycolor[2],
						gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->mycolor[3]);
	this->setangle(gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->myangle);
	this->x = gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->mypos[0];
	this->y = gameengine.players[gameengine.player_order[gameengine.currentPlayer]]->mypos[1];

	this->add(playernumber);

	unitButton = new menubutton(w-30, 20, 40, 0, h/2 - 65);
	this->add(unitButton); unitButton->hide = true;
	unitLabel = new Label("no unit selected", w-30, 20, 0, h/2 - 65);
	this->add(unitLabel);
	
	if(gameengine.movementPhase)
	{
		this->add(new Label("Movement Phase", 0.8*w, 20, 0, h/2-15));
		this->add(new menubutton(80,20,20,0,-h/2+15)); //Done-Button
		this->add(new menubutton(160,20,22,0,-h/2+40)); //Show-All-Button
	}
	else if(gameengine.attackPhase)
	{
		target_locked = false;
		this->add(new Label("Attack Phase", 0.8*w, 20, 0, h/2-15));
		this->add(new menubutton(80,20,21,0,-h/2+15)); //Done-Button (Attack Phase)
		this->add(new menubutton(160,20,23,0,-h/2+40)); //Show-All-Button (Attack Phase)
		for(int i = 0; i < 4; i++)
		{
			weapons.push_back(new Label("", w-30, 20, 12, h/2 - 90 - i*25));
			checkboxlist.push_back(new Checkbox(20,20, -w/2 + 12, h/2 - 90 - i*25,0,tex_storage.getCheckboxTexture()));
			this->add(checkboxlist[i]);
			checkboxlist[i]->hide = true;
			this->add(weapons[i]);
		}
		activeCheckbox = 0;
		unit_selected = false;
		target_selected = false;
		targetButton = new menubutton(w-30, 20, 0, h/2 - 200);
		infotext = new Label("Select Unit", w-30, 20, 0, h/2 - 200);
		this->add(infotext);
		hit_prob_text = new Label("", w-30,20,0,h/2 - 240);
		this->add(hit_prob_text);
		fireButton = new menubutton(60, 20, 24, 0, -h/2 + 65);
		fire_text = new Label("Fire", 60, 20, 0, -h/2 + 65);
		fired_text = new Label("", 120, 20, 0, -h/2 + 90);
		this->add(fired_text);
		ammo_text = new Label("", 100, 20, 0, h/2 - 270);
		this->add(ammo_text);

		this->add(targetButton); targetButton->hide = true;
		this->add(fireButton); fireButton->hide = true;
		this->add(fire_text); fire_text->hide = true;
	}
	this->add(new Label("Done",60,20,0,-h/2+15));
	this->add(new Label("Show all",140,20,0,-h/2+40));
}

void infoMenu::displayUnitInfo(unit* unit)
{
	if(gameengine.movementPhase)
	{
		if(gameengine.field->selectedUnit != 0) resetAll();
		unitButton->setType(40 + unit->getid());
		unitButton->hide = false; //this->add(unitButton);
		unitLabel->set(unit->getname());
	}
	else
	{
		unit_selected = true;
		myunit = unit;
		activeCheckbox = 0;
		checkboxlist[activeCheckbox]->set(1);
		unitButton->setType(40 + unit->getid());
		unitButton->hide = false; // this->add(unitButton);
		unitLabel->set(unit->getname());
		int longest_weapon_range = 0;
		number_of_checkboxes = unit->number_of_weapons;
		for(int i = 0; i < 4; i++)
		{
			if(i < unit->number_of_weapons)
			{
				weapons[i]->set(unit->weapons[i]->name);
				checkboxlist[i]->hide = false; //this->add(checkboxlist[i]);
				if(unit->weapons[i]->range_lng > longest_weapon_range && unit->weapons[i]->ammo != 0) 
					longest_weapon_range = unit->weapons[i]->range_lng;
			}
			else
				weapons[i]->set("empty");
			weapons[i]->hide = false; //this->add(weapons[i]);
		}
		gameengine.field->selectedWeapon = unit->weapons[0];
		gameengine.field->checkLOS(gameengine.field->selectedUnit->mycoords[0], gameengine.field->selectedUnit->mycoords[1], longest_weapon_range);
		infotext->set("Select Target");
		if(myunit->weapons[activeCheckbox]->ammo > -1) 
		{
			std::string ammotext = "ammo: ";
			ammotext.append(gameengine.IntToString(myunit->weapons[activeCheckbox]->ammo));
			ammo_text->set(ammotext);
		}
		if(myunit->weapons[activeCheckbox]->destroyed)
		{
			fired_text->set("DESTROYED!");
		}
	}
}

void infoMenu::displayTargetInfo(unit* unit)
{
	if(target_selected) resetTarget();
	target_selected = true;
	targetButton->setType(50 + unit->getid());
	targetButton->hide = false; //this->add(targetButton);
	infotext->set(unit->getname());
	infotext->raise();
	fireButton->hide = false; //this->add(fireButton);
	fire_text->hide = false; //this->add(fire_text);
}

void infoMenu::resetAll()
{
	if(gameengine.movementPhase)
	{
		unitLabel->set("no unit selected");
		unitButton->hide = true; //this->remove(unitButton);
	}
	else
	{
		if(unit_selected) 
		{
			unitButton->hide = true; //this->remove(unitButton);
			for(int i = 0; i < 4; i++)
				weapons[i]->hide = true; //this->remove(weapons[i]);
			for(int i = 0; i < number_of_checkboxes; i++)
				checkboxlist[i]->hide = true; //this->remove(checkboxlist[i]);				
			unit_selected = false;
		}
		if(target_selected) 
		{
			targetButton->hide = true;	// this->remove(targetButton);
			fireButton->hide = true;	// this->remove(fireButton);
			fire_text->hide = true;		// this->remove(fire_text);
		}
		target_selected = false;
		unitLabel->set("no unit selected");
		infotext->set("Select Unit");
		hit_prob_text->set("");
	}
}

void infoMenu::resetTarget()
{
	targetButton->hide = true;	
	fireButton->hide = true;
	fire_text->hide = true;	
	infotext->set("Select Target");
	hit_prob_text->set("");
}

void infoMenu::draw()
{
	//Checkboxes as radiobuttons
	if(gameengine.attackPhase && unit_selected)
	{
		bool activeCheckboxChanged = false;
		for(int i = 0; i < (signed)checkboxlist.size(); i++)
			if(checkboxlist[i]->get() && activeCheckbox != i)
			{
				checkboxlist[activeCheckbox]->set(0);
				activeCheckbox = i;
				activeCheckboxChanged = true;
				if(myunit->weapons[activeCheckbox]->fired) fired_text->set("F I R E D!");
				else if(myunit->weapons[activeCheckbox]->destroyed) fired_text->set("DESTROYED!");
				else fired_text->set("");
				if(myunit->weapons[activeCheckbox]->ammo > -1) 
				{
					std::string ammotext = "ammo: ";
					ammotext.append(gameengine.IntToString(myunit->weapons[activeCheckbox]->ammo));
					ammo_text->set(ammotext);
				}
				else ammo_text->set("");
				break;
			}
		checkboxlist[activeCheckbox]->set(1);
		if(activeCheckboxChanged)
		{
			gameengine.field->selectedWeapon = myunit->weapons[activeCheckbox];
			if(gameengine.field->selectedTargetUnit != 0)
				set_hit_prob_text();
		}		
	}

	Container::draw();
}

void infoMenu::set_hit_prob_text()
{	
	gameengine.calculate_modified_hit_number();
	std::string help = "Chance to Hit: ";
	help.append(gameengine.IntToString(gameengine.hit_probability));
	help.append("%");
	hit_prob_text->set(help);
}

void infoMenu::fire_weapon()
{
	if(myunit->weapons[activeCheckbox]->fired) 
	{
		this->dropWarning("Weapon already fired this round!");
		return;
	}
	if(myunit->weapons[activeCheckbox]->ammo == 0)
	{
		this->dropWarning("No ammo available for this weapon!");
		return;
	}
	if(myunit->weapons[activeCheckbox]->destroyed)
	{
		this->dropWarning("Weapon is already destroyed!");
		return;
	}
	target_locked = true;
	myunit->weapons[activeCheckbox]->fired = true;
	fired_text->set("F I R E D!");
	if(myunit->weapons[activeCheckbox]->ammo > -1)
	{
		myunit->weapons[activeCheckbox]->ammo--;
		std::string ammotext = "ammo: ";
		ammotext.append(gameengine.IntToString(myunit->weapons[activeCheckbox]->ammo));
		ammo_text->set(ammotext);
	}
	//save current_round_target of this unit for later use
	myunit->current_round_target = gameengine.field->selectedTargetUnit;

	//save hit_location of selected weapon for later use; "-1" if attack missed
	myunit->weapons[activeCheckbox]->current_round_hit_location = gameengine.get_hit_location();
}
