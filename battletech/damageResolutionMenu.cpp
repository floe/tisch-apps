/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "damageResolutionMenu.h"

damageResolutionMenu::damageResolutionMenu(int w, int h, int x, int y, double angle, RGBATexture* tex, int mode) : 
	menu(w, h, x, y, angle, tex, mode)
{
	mytex = tex_storage.getmenu();
	help = "R O U N D   ";
	help.append(gameengine.IntToString(gameengine.round));
	round_label = new Label(help.c_str(), 120, 30, 0, h/2 - 15);
	this->add(round_label);

	lineposition = h/2 - 50;
	line = 0;
	linespacing = 25;

	for(int i = 0; i < (signed)gameengine.players.size(); i++) //for each player:
	{
		for(int j = 0; j < (signed)gameengine.players[i]->units.size(); j++) //for each unit:
		{
			if(gameengine.players[i]->units[j]->current_round_target != 0) //this unit attacked this round?
			{ 
				if(gameengine.players[i]->units[j]->current_round_target->destroyed) break;
				addWeaponFireString(i,j);
				addTargetString(i,j);

				for(int k = 0; k < (signed)gameengine.players[i]->units[j]->weapons.size(); k++) //for each weapon
				{
					if(gameengine.players[i]->units[j]->current_round_target->destroyed) break;
					if(gameengine.players[i]->units[j]->weapons[k]->fired) //this weapon attacked this round?
					{
						if(gameengine.players[i]->units[j]->weapons[k]->current_round_hit_location == -1) //this weapon missed
						{
							addWeaponMissedString(i,j,k);
						}
						else //this weapon hit
						{ 
							addWeaponHitString(i,j,k);
						}
					}
				} //end of weaponloop
				labels.push_back(new Label("", 0,0));
				this->add(labels[line]);
				line++;
			} //end of unit attacked?
		} //end of unitloop
	} //end of playerloop
	done_button = new menubutton(60, 20, 30, 0, -h/2 + 15); 
	this->add(done_button);
	done_label = new Label("Done", 60, 20, 0, -h/2 + 15);
	this->add(done_label);
	
	if(line >= 19)
	{
		slider = new Slider(h - 60,20,+w/2-20,0,270,tex_storage.getSliderTexture());
		this->add(slider);
		slider_lines = line-19;
	}
}

void damageResolutionMenu::addWeaponFireString(int playernumber, int unitnumber)
{
	help = "Weapons fire for \"";
	help.append(gameengine.players[playernumber]->units[unitnumber]->getname());
	help.append("\"(Player ");
	help.append(gameengine.IntToString(playernumber+1));
	help.append("):");
	labels.push_back(new Label(help.c_str(), w-20, 20, -10, lineposition - line*linespacing));
	this->add(labels[line]); line++;
}

void damageResolutionMenu::addTargetString(int playernumber, int unitnumber)
{
	help = "target: \"";
	help.append(gameengine.players[playernumber]->units[unitnumber]->current_round_target->getname());
	help.append("\"(Player ");
	help.append(gameengine.IntToString(gameengine.players[playernumber]->units[unitnumber]->current_round_target->getmyplayer() + 1));
	help.append(")");
	labels.push_back(new Label(help.c_str(), w-40, 20, -10, lineposition - line*linespacing));
	this->add(labels[line]); line++;
}

void damageResolutionMenu::addWeaponMissedString(int playernumber, int unitnumber, int weaponnumber)
{
	help = "\"";
	help.append(gameengine.players[playernumber]->units[unitnumber]->weapons[weaponnumber]->name);
	help.append("\" misses.");
	labels.push_back(new Label(help.c_str(), w-40, 20, 0, lineposition - line*linespacing));
	this->add(labels[line]); line++;
}

void damageResolutionMenu::addWeaponHitString(int playernumber, int unitnumber, int weaponnumber)
{
	help = "\"";
	help.append(gameengine.players[playernumber]->units[unitnumber]->weapons[weaponnumber]->name);
	help.append("\" hits ");
	location = gameengine.players[playernumber]->units[unitnumber]->weapons[weaponnumber]->current_round_hit_location;
	location_name = gameengine.players[playernumber]->units[unitnumber]->current_round_target->getLocationName(location);
	help.append(location_name);
	help.append(" for ");
	int damage = gameengine.players[playernumber]->units[unitnumber]->weapons[weaponnumber]->damage;
	help.append(gameengine.IntToString(damage));
	help.append(" damage:");
	labels.push_back(new Label(help.c_str(), w-40, 20, 0, lineposition - line*linespacing));
	this->add(labels[line]); line++;

	process_damage(damage, playernumber, unitnumber);
	
}

void damageResolutionMenu::process_damage(int damage, int playernumber, int unitnumber)
{
	help = "";
	if(gameengine.players[playernumber]->units[unitnumber]->current_round_target->hitpoints[location] == 0)
	{
		help = "";
		help.append(location_name);
		help.append(" already destroyed. ");
		help.append(gameengine.IntToString(damage));
		help.append(" damage transfered to ");
		location = gameengine.players[playernumber]->units[unitnumber]->current_round_target->damage_transfer[location];
		location_name = gameengine.players[playernumber]->units[unitnumber]->current_round_target->getLocationName(location);
		help.append(location_name);
		labels.push_back(new Label(help.c_str(), w-60, 20, 10, lineposition - line*linespacing));
		this->add(labels[line]); line++;

		process_damage(damage, playernumber, unitnumber);
		return;
	}
	gameengine.players[playernumber]->units[unitnumber]->current_round_target->hitpoints[location] -= damage;

	if(gameengine.players[playernumber]->units[unitnumber]->current_round_target->hitpoints[location] < 0) //need to transfer damage
	{
		int transfered_damage = -1 * gameengine.players[playernumber]->units[unitnumber]->current_round_target->hitpoints[location];
		gameengine.players[playernumber]->units[unitnumber]->current_round_target->hitpoints[location] = 0;
		help.append("0 armor remaining on ");
		help.append(location_name);
		help.append(", ");
		help.append(location_name);
		help.append(" destroyed!");
		labels.push_back(new Label(help.c_str(), w-60, 20, 10, lineposition - line*linespacing));
		this->add(labels[line]); line++;
	
		showDestruction(playernumber, unitnumber);

		if(location != 0 && location != 1) //not Head or Center Torso
		{
			help = "";
			help.append(gameengine.IntToString(transfered_damage));
			help.append(" damage transfered to ");
			location = gameengine.players[playernumber]->units[unitnumber]->current_round_target->damage_transfer[location];
			location_name = gameengine.players[playernumber]->units[unitnumber]->current_round_target->getLocationName(location);
			help.append(location_name);
			labels.push_back(new Label(help.c_str(), w-60, 20, 10, lineposition - line*linespacing));
			this->add(labels[line]); line++;

			process_damage(transfered_damage, playernumber, unitnumber);
			
		}
	}
	else //no damage is transferred
	{
		help = gameengine.IntToString(gameengine.players[playernumber]->units[unitnumber]->current_round_target->hitpoints[location]);
		help.append(" armor remaining on ");
		help.append(location_name);
		help.append(".");
		labels.push_back(new Label(help.c_str(), w-60, 20, 10, lineposition - line*linespacing));
		this->add(labels[line]); line++;
		if(gameengine.players[playernumber]->units[unitnumber]->current_round_target->hitpoints[location] == 0)
			showDestruction(playernumber, unitnumber);
	}
}

void damageResolutionMenu::showDestruction(int playernumber, int unitnumber)
{
	help = "";
	if(location == 0 || location == 1) //Head or Center Torso destroyed
	{
		gameengine.players[playernumber]->units[unitnumber]->current_round_target->destroyed = true;
		help.append(" *** ");
		help.append(gameengine.players[playernumber]->units[unitnumber]->current_round_target->getname());
		help.append("\"(Player ");
		help.append(gameengine.IntToString(gameengine.players[playernumber]->units[unitnumber]->current_round_target->getmyplayer() + 1));
		help.append(") destroyed! ***");
	}
	else if(location == 2) //Left Torso destroyed
	{
		gameengine.players[playernumber]->units[unitnumber]->current_round_target->hitpoints[4] = 0;
		help.append(" Can't use left arm anymore!");
		for(int a = 0; a < (signed)gameengine.players[playernumber]->units[unitnumber]->current_round_target->weapons.size(); a++)
		{
			if(gameengine.players[playernumber]->units[unitnumber]->current_round_target->weapons[a]->location == 4)//weapons of Left Arm
			{
				gameengine.players[playernumber]->units[unitnumber]->current_round_target->weapons[a]->destroyed = true;
				help.append(" Can't use ");
				help.append(gameengine.players[playernumber]->units[unitnumber]->current_round_target->weapons[a]->name);
				help.append(" anymore! ");
			}
		}
	}
	else if(location == 3) //Right Torso destroyed
	{
		gameengine.players[playernumber]->units[unitnumber]->current_round_target->hitpoints[5] = 0;
		help.append(" Can't use right arm anymore!");
		for(int a = 0; a < (signed)gameengine.players[playernumber]->units[unitnumber]->current_round_target->weapons.size(); a++)
		{
			if(gameengine.players[playernumber]->units[unitnumber]->current_round_target->weapons[a]->location == 5)//weapons of Right Arm
			{
				gameengine.players[playernumber]->units[unitnumber]->current_round_target->weapons[a]->destroyed = true;
				help.append(" Can't use ");
				help.append(gameengine.players[playernumber]->units[unitnumber]->current_round_target->weapons[a]->name);
				help.append(" anymore! ");
			}
		}
	}
	else if(location == 6 || location == 7) //Leg destroyed
	{
		gameengine.players[playernumber]->units[unitnumber]->current_round_target->walkspeed = 0;
		gameengine.players[playernumber]->units[unitnumber]->current_round_target->runspeed = 0;
		help.append(" Can't move anymore!");
	}

	if(location != 0 && location != 1)
	{
		for(int a = 0; a < (signed)gameengine.players[playernumber]->units[unitnumber]->current_round_target->weapons.size(); a++)
		{
			if(gameengine.players[playernumber]->units[unitnumber]->current_round_target->weapons[a]->location == location)
			{
				gameengine.players[playernumber]->units[unitnumber]->current_round_target->weapons[a]->destroyed = true;
				help.append(" Can't use ");
				help.append(gameengine.players[playernumber]->units[unitnumber]->current_round_target->weapons[a]->name);
				help.append(" anymore! ");
			}
		}
	}
	labels.push_back(new Label(help.c_str(), w-60, 20, 10, lineposition - line*linespacing));
	this->add(labels[line]); line++;
}

void damageResolutionMenu::draw()
{
	
	enter();	
	Tile::paint();
	
	glMatrixMode(GL_MODELVIEW_MATRIX);
	glPushMatrix();
	if(line >= 20) slider_pos =  slider_lines*slider->getpos();
	else slider_pos = 0;
	glTranslated(0,((int)slider_pos)*linespacing,1);
	for(int i = 0 + slider_pos; i < 19 + slider_pos && i < (signed)labels.size(); i++)
	{
		labels[i]->draw();
	}
	glPopMatrix();
	if(line >= 19) slider->draw();
	round_label->draw();
	done_button->draw();
	done_label->draw();
	leave();
}