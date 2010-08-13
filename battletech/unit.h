/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#ifndef UNIT_H
#define UNIT_H
#include <stdio.h>
#include "xmlParser/xmlParser.h"
#include <string>
#include <tisch.h>
#include <Texture.h>

class weapon
{
public:
	weapon();
	void createWeapon(const char* weapon_number);
	std::string name;
	std::string type;
	int damage, range_sht, range_med, range_lng, ammo, location;
	int damage_sht, damage_med, damage_lng;
	bool fired;
	bool destroyed;
	int current_round_hit_location;
};

class unit
{
public:
	unit() {};
	virtual std::string getname(){ return name; };
	virtual int getid() { return id; };
	virtual RGBATexture* gettexture() { return tex; };
	virtual int getmyplayer(){ return myplayer; };
	virtual std::string getLocationName(int location_number){ return ""; };
	
	int to_hit_modifier_sht, to_hit_modifier_med, to_hit_modifier_lng;
	int number_of_weapons;
	std::vector<weapon*> weapons;
	int hit_location[13];
	int hitpoints[8];
	int max_hitpoints[8];
	int damage_transfer[8];

	bool deployed;
	int mycoords[2];
	int facing;
	bool moved;
	int num_of_hexes_moved;
	bool attacked;
	int terrain_modifier;
	int attacker_movement_modifier;
	unit* current_round_target;
	int walkspeed, runspeed, base_to_hit, tonnage;
	bool destroyed;

protected:
	std::string name;
	int id;
	RGBATexture* tex;
	int myplayer;
};

class mech : public unit
{
public:
	mech(const char* name, int id, int playernumber, RGBATexture* tex);
	static int getLocation(const char* loc);
	std::string getLocationName(int location_number);
};
#endif

