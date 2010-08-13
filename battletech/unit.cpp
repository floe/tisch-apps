/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "unit.h"
#include "engine.h"

XMLNode xmlMainNode, xmlNode;

weapon::weapon(){ fired = false; current_round_hit_location = -1; destroyed = false; }

void weapon::createWeapon(const char* weapon_number)
{
	xmlNode = xmlNode.getChildNode(weapon_number);
	name		= (char*)xmlNode.getChildNode("weapon_name").getText();
	location	= mech::getLocation(xmlNode.getChildNode("weapon_loc").getText());
	ammo		= strtol(xmlNode.getChildNode("weapon_ammo").getText(), (char**)0, 10);
	damage		= strtol(xmlNode.getChildNode("weapon_damage").getText(), (char**)0, 10);

	if(damage == -1)
	{
		xmlNode = xmlNode.getChildNode("weapon_type");
		type = xmlNode.getText();
		if(type[0] == 'V')
		{
			damage_sht = strtol(xmlNode.getChildNode("weapon_damage_sht").getText(), (char**)0, 10);
			damage_med = strtol(xmlNode.getChildNode("weapon_damage_med").getText(), (char**)0, 10);
			damage_lng = strtol(xmlNode.getChildNode("weapon_damage_lng").getText(), (char**)0, 10);
		}
		else
			damage = strtol(xmlNode.getChildNode("weapon_damage").getText(), (char**)0, 10);
		xmlNode = xmlNode.getParentNode();
	}
	else
		type = "N"; 

	xmlNode = xmlNode.getChildNode("weapon_range");
	range_sht	= strtol(xmlNode.getChildNode("sht").getText(), (char**)0, 10);
	range_med	= strtol(xmlNode.getChildNode("med").getText(), (char**)0, 10);
	range_lng	= strtol(xmlNode.getChildNode("lng").getText(), (char**)0, 10);
	xmlNode = xmlNode.getParentNode();

	xmlNode = xmlNode.getParentNode();

}

mech::mech(const char* name, int id, int playernumber, RGBATexture* tex) : unit()
{
	this->name = name;
	this->id = id;
	this->tex = tex;
	this->myplayer = playernumber;
	this->current_round_target = 0;
	deployed = false;
	destroyed = false;
	facing = 0;
	moved = false;
	num_of_hexes_moved = 0;
	attacked = false;
	terrain_modifier = 0;
	attacker_movement_modifier = 0;
	xmlMainNode = XMLNode::openFileHelper("units.xml");
	xmlNode = xmlMainNode.getChildNode("unit").getChildNode(name);
	walkspeed			= strtol(xmlNode.getChildNode("move").getChildNode("walking").getText(), (char**)0, 10);
	runspeed			= strtol(xmlNode.getChildNode("move").getChildNode("running").getText(), (char**)0, 10);
	tonnage				= strtol(xmlNode.getChildNode("tonnage").getText(), (char**)0, 10);
	base_to_hit			= strtol(xmlNode.getChildNode("base_to_hit").getText(), (char**)0, 10);
	to_hit_modifier_sht	= strtol(xmlNode.getChildNode("to_hit_modifier").getChildNode("sht").getText(), (char**)0, 10);
	to_hit_modifier_med	= strtol(xmlNode.getChildNode("to_hit_modifier").getChildNode("med").getText(), (char**)0, 10);
	to_hit_modifier_lng	= strtol(xmlNode.getChildNode("to_hit_modifier").getChildNode("lng").getText(), (char**)0, 10);
	
	xmlNode = xmlNode.getChildNode("hitpoints");
	hitpoints[0]	= strtol(xmlNode.getChildNode("Head").getText(), (char**)0, 10);
	hitpoints[1]	= strtol(xmlNode.getChildNode("CT").getText(), (char**)0, 10);
	hitpoints[2]	= strtol(xmlNode.getChildNode("LT").getText(), (char**)0, 10);
	hitpoints[3]	= strtol(xmlNode.getChildNode("RT").getText(), (char**)0, 10);
	hitpoints[4]	= strtol(xmlNode.getChildNode("LA").getText(), (char**)0, 10);
	hitpoints[5]	= strtol(xmlNode.getChildNode("RA").getText(), (char**)0, 10);
	hitpoints[6]	= strtol(xmlNode.getChildNode("LL").getText(), (char**)0, 10);
	hitpoints[7]	= strtol(xmlNode.getChildNode("RL").getText(), (char**)0, 10);
	for(int i = 0; i < 8; i++) max_hitpoints[i] = hitpoints[i];
	xmlNode = xmlNode.getParentNode();

	xmlNode = xmlNode.getChildNode("hit_locations");
	hit_location[0]		= -1;
	hit_location[1]		= -1;
	hit_location[2]		= getLocation(xmlNode.getChildNode("two").getText());
	hit_location[3]		= getLocation(xmlNode.getChildNode("three").getText());
	hit_location[4]		= getLocation(xmlNode.getChildNode("four").getText());
	hit_location[5]		= getLocation(xmlNode.getChildNode("five").getText());
	hit_location[6]		= getLocation(xmlNode.getChildNode("six").getText());
	hit_location[7]		= getLocation(xmlNode.getChildNode("seven").getText());
	hit_location[8]		= getLocation(xmlNode.getChildNode("eight").getText());
	hit_location[9]		= getLocation(xmlNode.getChildNode("nine").getText());
	hit_location[10]	= getLocation(xmlNode.getChildNode("ten").getText());
	hit_location[11]	= getLocation(xmlNode.getChildNode("eleven").getText());
	hit_location[12]	= getLocation(xmlNode.getChildNode("twelve").getText());
	xmlNode = xmlNode.getParentNode();

	xmlNode = xmlNode.getChildNode("damage_transfer");
	damage_transfer[0] = -1;
	damage_transfer[1] = -1;
	damage_transfer[2] = getLocation(xmlNode.getChildNode("LT").getText());
	damage_transfer[3] = getLocation(xmlNode.getChildNode("RT").getText());
	damage_transfer[4] = getLocation(xmlNode.getChildNode("LA").getText());
	damage_transfer[5] = getLocation(xmlNode.getChildNode("RA").getText());
	damage_transfer[6] = getLocation(xmlNode.getChildNode("LL").getText());
	damage_transfer[7] = getLocation(xmlNode.getChildNode("RL").getText());
	xmlNode = xmlNode.getParentNode();

	number_of_weapons = strtol(xmlNode.getChildNode("number_of_weapons").getText(), (char**)0, 10);
	if(number_of_weapons > 0) { weapons.push_back(new weapon()); weapons[0]->createWeapon("weapon0"); }
	if(number_of_weapons > 1) { weapons.push_back(new weapon()); weapons[1]->createWeapon("weapon1"); }
	if(number_of_weapons > 2) { weapons.push_back(new weapon()); weapons[2]->createWeapon("weapon2"); }
	if(number_of_weapons > 3) { weapons.push_back(new weapon()); weapons[3]->createWeapon("weapon3"); }
	
	this->name = gameengine.removeUnderscore(this->name);

}

int mech::getLocation(const char* loc)
{
		 if( strcmp(loc, "Head") == 0)	return 0;
	else if( strcmp(loc, "CT") == 0)	return 1;
	else if( strcmp(loc, "LT") == 0)	return 2;
	else if( strcmp(loc, "RT") == 0)	return 3;
	else if( strcmp(loc, "LA") == 0)	return 4;
	else if( strcmp(loc, "RA") == 0)	return 5;
	else if( strcmp(loc, "LL") == 0)	return 6;
	else if( strcmp(loc, "RL") == 0)	return 7;
	else return -1;
}

std::string mech::getLocationName(int location_number)
{
		 if(location_number == 0) return "Head";
	else if(location_number == 1) return "Center Torso";
	else if(location_number == 2) return "Left Torso";
	else if(location_number == 3) return "Right Torso";
	else if(location_number == 4) return "Left Arm";
	else if(location_number == 5) return "Right Arm";
	else if(location_number == 6) return "Left Leg";
	else if(location_number == 7) return "Right Leg";
	else return "error";
}
