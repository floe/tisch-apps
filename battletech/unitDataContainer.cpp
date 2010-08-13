/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "unitDataContainer.h"
#include "menubutton.h"

unitDataContainer::unitDataContainer(int w, int h, int unittype, bool _attacker, int x, int y, double angle, RGBATexture* tex, int mode): 
	Container(w,h,x,y,angle,tex,mode),
	type(unittype), attacker(_attacker)
{
	mytex = tex_storage.getUnitDataSheet(unittype);	

	this->add(new menubutton(100,40,9,-40,-h/2+20));
	this->add(new Label("Close",100,40,-40,-h/2+20));
}

void unitDataContainer::draw()
{
	Container::draw();

	if(gameengine.movementPhase || gameengine.attackPhase)
	{
		enter();
		glLineWidth(3);
		glColor4f(1,0,0,1);
		drawMissingHitpoints();
		leave();
	}
}

void unitDataContainer::drawMissingHitpoints()
{
	unit* unit;
	if(attacker)
		unit = gameengine.field->selectedTargetUnit;
	else
		unit = gameengine.field->selectedUnit;

	switch(type)
	{
	case 0: drawUnit_Enforcer(unit); break; //"ENF-4R Enforcer"
	case 1: drawUnit_Hunchback(unit); break; //"HBK-4G Hunchback"
	}
}

void unitDataContainer::drawUnit_Enforcer(unit* unit)
{
	d = 3;
	//Head
	start_x = 160; start_y = 80.85; dist_x = 6;
	for(int i = 0; i < unit->max_hitpoints[0] - unit->hitpoints[0]; i++)
	{
		if(i < 6)
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x + i%3 * dist_x, start_y + (int)(i/3) * dist_x);
			glVertex2d(start_x + i%3 * dist_x + d, start_y + (int)(i/3) * dist_x + d);
			glEnd();
		}
		else if(i < 8)
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x + i%2 * dist_x + dist_x/2, start_y + 1.6 * dist_x);
			glVertex2d(start_x + i%2 * dist_x + dist_x/2 + d, start_y + 1.6 * dist_x + d);
			glEnd();
		}
		else
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x + dist_x, start_y + 2.4 * dist_x);
			glVertex2d(start_x + dist_x + d, start_y + 2.4 * dist_x + d);
			glEnd();
		}
	}
	//end of Head
	//Center Torso
	start_x = 166; start_y = -6.5208; dist_x = 6; dist_y = 6.15;
	for(int i = 0; i < unit->max_hitpoints[1] - unit->hitpoints[1]; i++)
	{
		if(i < 5)
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x, start_y + i * dist_x);
			glVertex2d(start_x + d, start_y + i * dist_x + d);
			glEnd();
		}
		else if(i < 8)
		{
			start_x = 160.64; start_y = 28.9278; dist_x = 5;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-5) * dist_x, start_y);
			glVertex2d(start_x + (i-5) * dist_x + d, start_y + d);
			glEnd();
		}
		else
		{
			start_y = 40.8408;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-8)%3 * dist_x, start_y + (int)((i-8)/3) * dist_y);
			glVertex2d(start_x + (i-8)%3 * dist_x + d, start_y + (int)((i-8)/3) * dist_y + d);
			glEnd();
		}
	}
	//end of Center Torso
	//Left Torso
	start_x = 142.176; start_y = 4.5408; dist_x = 6; dist_y = 6.6; d = 3.5;
	for(int i = 0; i < unit->max_hitpoints[2] - unit->hitpoints[2]; i++)
	{
		if(i < 4)
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x, start_y + i * dist_y);
			glVertex2d(start_x + d, start_y + i * dist_y + d);
			glEnd();
		}
		else if(i < 5)
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x - 1, start_y + i * dist_y);
			glVertex2d(start_x - 1 + d, start_y + i * dist_y + d);
			glEnd();
		}
		else if(i < 7)
		{
			dist_y = 6.8;
			glBegin(GL_LINES); 
			glVertex2d(start_x - 3 + (i%2)*0.5, start_y + i * dist_y);
			glVertex2d(start_x - 3 + (i%2)*0.5 + d, start_y + i * dist_y + d);
			glEnd();
		}
		else
		{
			start_x = 129.248; start_y = 52.4634;  dist_x = 9.3; dist_y = 6.8; d = 3.5;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-7)%2 * dist_x, start_y + (int)((i-7)/2) * dist_y);
			glVertex2d(start_x + (i-7)%2 * dist_x + d, start_y + (int)((i-7)/2) * dist_y + d);
			glEnd();
		}
	}
	//end of Left Torso
	//Right Torso
	start_x = 189.184; start_y = 4.5408; dist_x = 6; dist_y = 6.6; d = 3.5;
	for(int i = 0; i < unit->max_hitpoints[3] - unit->hitpoints[3]; i++)
	{
		if(i < 4)
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x, start_y + i * dist_y);
			glVertex2d(start_x + d, start_y + i * dist_y + d);
			glEnd();
		}
		else if(i < 5)
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x + 1, start_y + i * dist_y);
			glVertex2d(start_x + 1 + d, start_y + i * dist_y + d);
			glEnd();
		}
		else if(i < 7)
		{
			dist_y = 6.8;
			glBegin(GL_LINES); 
			glVertex2d(start_x + 3 - (i%2)*0.5, start_y + i * dist_y);
			glVertex2d(start_x + 3 - (i%2)*0.5 + d, start_y + i * dist_y + d);
			glEnd();
		}
		else
		{
			start_x = 192.4992; start_y = 52.4634; dist_x = 9.3; dist_y = 6.8; d = 3.5;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-7)%2 * dist_x, start_y + (int)((i-7)/2) * dist_y);
			glVertex2d(start_x + (i-7)%2 * dist_x + d, start_y + (int)((i-7)/2) * dist_y + d);
			glEnd();
		}
	}
	//end of Right Torso
	//Left Arm
	start_x = 98.7264; start_y = 6.5208; dist_x = 0.7; dist_y = 7; d = 3.8;
	for(int i = 0; i < unit->max_hitpoints[4] - unit->hitpoints[4]; i++)
	{
		if(i < 5)
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x + i * dist_x, start_y + i * dist_y);
			glVertex2d(start_x + i * dist_x + d, start_y + i * dist_y + d);
			glEnd();
		}
		else if(i < 7)
		{
			start_x = 102.5728; start_y = 48.51;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-5) * dist_x, start_y + (i-5) * dist_y);
			glVertex2d(start_x + (i-5) * dist_x + d, start_y + (i-5) * dist_y + d);
			glEnd();
		}
		else if(i < 10)
		{
			start_x = 103.9728 ; start_y = 62.51; dist_x = 7; dist_y = -0.5;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-8) * dist_x, start_y + (i-8) * dist_y);
			glVertex2d(start_x + (i-8) * dist_x + d, start_y + (i-8) * dist_y + d);
			glEnd();
		}
		else if(i < 13)
		{
			start_x = 104.6728 ; start_y = 69.51; dist_x = 7; dist_y = -0.5;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-11) * dist_x, start_y + (i-11) * dist_y);
			glVertex2d(start_x + (i-11) * dist_x + d, start_y + (i-11) * dist_y + d);
			glEnd();
		}
		else
		{
			dist_x = 0.7; dist_y = 7.5;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-12) * dist_x, start_y + (i-12) * dist_y);
			glVertex2d(start_x + (i-12) * dist_x + d, start_y + (i-12) * dist_y + d);
			glEnd();
		}
	}
	//end of Left Arm
	//Right Arm
	start_x = 232.1024; start_y = 6.5208; dist_x = -0.7; dist_y = 7; d = 3.8;
	for(int i = 0; i < unit->max_hitpoints[5] - unit->hitpoints[5]; i++)
	{
		if(i < 5)
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x + i * dist_x, start_y + i * dist_y);
			glVertex2d(start_x + i * dist_x + d, start_y + i * dist_y + d);
			glEnd();
		}
		else if(i < 7)
		{
			start_x = 229.0752; start_y = 48.51;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-5) * dist_x, start_y + (i-5) * dist_y);
			glVertex2d(start_x + (i-5) * dist_x + d, start_y + (i-5) * dist_y + d);
			glEnd();
		}
		else if(i < 10)
		{
			start_x = 227.6752 ; start_y = 62.51; dist_x = 7; dist_y = 0.5;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-8) * dist_x, start_y + (i-8) * dist_y);
			glVertex2d(start_x + (i-8) * dist_x + d, start_y + (i-8) * dist_y + d);
			glEnd();
		}
		else if(i < 13)
		{
			start_x = 226.9752 ; start_y = 69.51; dist_x = 7; dist_y = 0.5;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-11) * dist_x, start_y + (i-11) * dist_y);
			glVertex2d(start_x + (i-11) * dist_x + d, start_y + (i-11) * dist_y + d);
			glEnd();
		}
		else
		{
			dist_x = -0.7; dist_y = 7.5;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-12) * dist_x, start_y + (i-12) * dist_y);
			glVertex2d(start_x + (i-12) * dist_x + d, start_y + (i-12) * dist_y + d);
			glEnd();
		}
	}
	//end of Right Arm
	//Left Leg
	start_x = 117; start_y = -96.5; dist_x = 7; dist_y = 6.7; d = 3.8;
	for(int i = 0; i < unit->max_hitpoints[6] - unit->hitpoints[6]; i++)
	{
		if(i < 10) 
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i%2) * dist_x + (i/2)*1.5, start_y + (int)(i/2) * dist_y + (i%2)*-2);
			glVertex2d(start_x + (i%2) * dist_x + d + (i/2)*1.5, start_y + (int)(i/2) * dist_y + d + (i%2)*-2);
			glEnd();
		}
		else
		{
			start_x = 128; start_y = -48.774;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i%2) * dist_x + ((i-10)/2)*1.5, start_y + (int)((i-10)/2) * dist_y + (i%2)*-2);
			glVertex2d(start_x + (i%2) * dist_x + d + ((i-10)/2)*1.5, start_y + (int)((i-10)/2) * dist_y + d + (i%2)*-2);
			glEnd();
		}
	}
	//end of Left Leg
	//Right Leg
	start_x = 208.1728; start_y = -98.406; dist_x = 7; dist_y = 6.7; d = 3.8;
	for(int i = 0; i < unit->max_hitpoints[7] - unit->hitpoints[7]; i++)
	{
		if(i < 10) 
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i%2) * dist_x - (i/2)*1.5, start_y + (int)(i/2) * dist_y + (i%2)*2);
			glVertex2d(start_x + (i%2) * dist_x + d - (i/2)*1.5, start_y + (int)(i/2) * dist_y + d + (i%2)*2);
			glEnd();
		}
		else
		{
			start_x = 197.12; start_y = -50.4768;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i%2) * dist_x - ((i-10)/2)*1.5, start_y + (int)((i-10)/2) * dist_y + (i%2)*2);
			glVertex2d(start_x + (i%2) * dist_x + d - ((i-10)/2)*1.5, start_y + (int)((i-10)/2) * dist_y + d + (i%2)*2);
			glEnd();
		}
	}
	//end of Right Leg
}

void unitDataContainer::drawUnit_Hunchback(unit* unit)
{
	d = 3;
	//Head
	start_x = 160; start_y = 80.85; dist_x = 5.5; dist_y = 5;
	for(int i = 0; i < unit->max_hitpoints[0] - unit->hitpoints[0]; i++)
	{
		if(i < 6)
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x + i%3 * dist_x, start_y + (int)(i/3) * dist_y);
			glVertex2d(start_x + i%3 * dist_x + d, start_y + (int)(i/3) * dist_y + d);
			glEnd();
		}
		else if(i < 8)
		{
			dist_y = 5.8;
			glBegin(GL_LINES); 
			glVertex2d(start_x + i%2 * dist_x + dist_x/2, start_y + 1.6 * dist_y);
			glVertex2d(start_x + i%2 * dist_x + dist_x/2 + d, start_y + 1.6 * dist_y + d);
			glEnd();
		}
		else
		{
			dist_y = 5.6;
			glBegin(GL_LINES); 
			glVertex2d(start_x + dist_x, start_y + 2.4 * dist_y);
			glVertex2d(start_x + dist_x + d, start_y + 2.4 * dist_y + d);
			glEnd();
		}
	}
	//end of Head
	//Center Torso
	start_x = 166; start_y = -6.5208; dist_x = 6; dist_y = 6.15;
	for(int i = 0; i < unit->max_hitpoints[1] - unit->hitpoints[1]; i++)
	{
		if(i < 1)
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x, start_y + i * dist_x);
			glVertex2d(start_x + d, start_y + i * dist_x + d);
			glEnd();
		}
		else if(i < 11)
		{
			start_x = 160.4992; start_y = -1.219; dist_x = 9.5; dist_y = 6;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-1)%2 * dist_x, start_y + (i-1)/2 * dist_y);
			glVertex2d(start_x + (i-1)%2 * dist_x + d, start_y + (i-1)/2 * dist_y + d);
			glEnd();
		}
		else
		{
			start_x = 155.52; start_y = 40.3128; dist_x = 9.8;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-11)%3 * dist_x, start_y + (int)((i-11)/3) * dist_y);
			glVertex2d(start_x + (i-11)%3 * dist_x + d, start_y + (int)((i-11)/3) * dist_y + d);
			glEnd();
		}
	}
	//end of Center Torso
	//Left Torso
	start_x = 142.176; start_y = 4.5408; dist_x = 6; dist_y = 6.6; d = 3.5;
	for(int i = 0; i < unit->max_hitpoints[2] - unit->hitpoints[2]; i++)
	{
		if(i < 4)
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x, start_y + i * dist_y);
			glVertex2d(start_x + d, start_y + i * dist_y + d);
			glEnd();
		}
		else if(i < 5)
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x - 1, start_y + i * dist_y);
			glVertex2d(start_x - 1 + d, start_y + i * dist_y + d);
			glEnd();
		}
		else
		{
			start_x = 124.4352; start_y = 51.3876;  dist_x = 9.6; dist_y = 6.8; d = 3.5;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-5)%3 * dist_x, start_y + (int)((i-5)/3) * dist_y);
			glVertex2d(start_x + (i-5)%3 * dist_x + d, start_y + (int)((i-5)/3) * dist_y + d);
			glEnd();
		}
	}
	//end of Left Torso
	//Right Torso
	start_x = 189.184; start_y = 4.5408; dist_x = 6; dist_y = 6.6; d = 3.5;
	for(int i = 0; i < unit->max_hitpoints[3] - unit->hitpoints[3]; i++)
	{
		if(i < 4)
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x, start_y + i * dist_y);
			glVertex2d(start_x + d, start_y + i * dist_y + d);
			glEnd();
		}
		else if(i < 5)
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x + 1, start_y + i * dist_y);
			glVertex2d(start_x + 1 + d, start_y + i * dist_y + d);
			glEnd();
		}
		else
		{
			start_x = 187.2; start_y = 51.3876; dist_x = 9.6; dist_y = 6.8; d = 3.5;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-5)%3 * dist_x, start_y + (int)((i-5)/3) * dist_y);
			glVertex2d(start_x + (i-5)%3 * dist_x + d, start_y + (int)((i-5)/3) * dist_y + d);
			glEnd();
		}
	}
	//end of Right Torso
	//Left Arm
	start_x = 98.7264; start_y = 6.5208; dist_x = 0.7; dist_y = 7; d = 3.8;
	for(int i = 0; i < unit->max_hitpoints[4] - unit->hitpoints[4]; i++)
	{
		if(i < 5)
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x + i * dist_x, start_y + i * dist_y);
			glVertex2d(start_x + i * dist_x + d, start_y + i * dist_y + d);
			glEnd();
		}
		else if(i < 7)
		{
			start_x = 102.5728; start_y = 48.51;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-5) * dist_x, start_y + (i-5) * dist_y);
			glVertex2d(start_x + (i-5) * dist_x + d, start_y + (i-5) * dist_y + d);
			glEnd();
		}
		else if(i < 10)
		{
			start_x = 103.9728 ; start_y = 62.51; dist_x = 6.5; dist_y = -0.5;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-8) * dist_x, start_y + (i-8) * dist_y);
			glVertex2d(start_x + (i-8) * dist_x + d, start_y + (i-8) * dist_y + d);
			glEnd();
		}
		else if(i < 13)
		{
			start_x = 104.6728 ; start_y = 69.51;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-11) * dist_x, start_y + (i-11) * dist_y);
			glVertex2d(start_x + (i-11) * dist_x + d, start_y + (i-11) * dist_y + d);
			glEnd();
		}
		else
		{
			start_x = 105.3728 ; start_y = 76.51;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-14) * dist_x, start_y + (i-14) * dist_y);
			glVertex2d(start_x + (i-14) * dist_x + d, start_y + (i-14) * dist_y + d);
			glEnd();
		}
	}
	//end of Left Arm
	//Right Arm
	start_x = 232.1024; start_y = 6.5208; dist_x = -0.7; dist_y = 7; d = 3.8;
	for(int i = 0; i < unit->max_hitpoints[5] - unit->hitpoints[5]; i++)
	{
		if(i < 5)
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x + i * dist_x, start_y + i * dist_y);
			glVertex2d(start_x + i * dist_x + d, start_y + i * dist_y + d);
			glEnd();
		}
		else if(i < 7)
		{
			start_x = 229.0752; start_y = 48.51;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-5) * dist_x, start_y + (i-5) * dist_y);
			glVertex2d(start_x + (i-5) * dist_x + d, start_y + (i-5) * dist_y + d);
			glEnd();
		}
		else if(i < 10)
		{
			start_x = 227.6752 ; start_y = 62.51; dist_x = 6.5; dist_y = 0.5;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-8) * dist_x, start_y + (i-8) * dist_y);
			glVertex2d(start_x + (i-8) * dist_x + d, start_y + (i-8) * dist_y + d);
			glEnd();
		}
		else if(i < 13)
		{
			start_x = 226.9752 ; start_y = 69.51;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-11) * dist_x, start_y + (i-11) * dist_y);
			glVertex2d(start_x + (i-11) * dist_x + d, start_y + (i-11) * dist_y + d);
			glEnd();
		}
		else
		{
			start_x = 226.2752 ; start_y = 76.51;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i-14) * dist_x, start_y + (i-14) * dist_y);
			glVertex2d(start_x + (i-14) * dist_x + d, start_y + (i-14) * dist_y + d);
			glEnd();
		}
	}
	//end of Right Arm
	//Left Leg
	start_x = 117; start_y = -96.5; dist_x = 7; dist_y = 6.7; d = 3.8;
	for(int i = 0; i < unit->max_hitpoints[6] - unit->hitpoints[6]; i++)
	{
		if(i < 10) 
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i%2) * dist_x + (i/2)*1.5, start_y + (int)(i/2) * dist_y + (i%2)*-2);
			glVertex2d(start_x + (i%2) * dist_x + d + (i/2)*1.5, start_y + (int)(i/2) * dist_y + d + (i%2)*-2);
			glEnd();
		}
		else
		{
			start_x = 127.5; start_y = -48.874;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i%2) * dist_x + ((i-10)/2)*1.5, start_y + (int)((i-10)/2) * dist_y + (i%2)*-2);
			glVertex2d(start_x + (i%2) * dist_x + d + ((i-10)/2)*1.5, start_y + (int)((i-10)/2) * dist_y + d + (i%2)*-2);
			glEnd();
		}
	}
	//end of Left Leg
	//Right Leg
	start_x = 208.1728; start_y = -98.406; dist_x = 7; dist_y = 6.7; d = 3.8;
	for(int i = 0; i < unit->max_hitpoints[7] - unit->hitpoints[7]; i++)
	{
		if(i < 10) 
		{
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i%2) * dist_x - (i/2)*1.5, start_y + (int)(i/2) * dist_y + (i%2)*2);
			glVertex2d(start_x + (i%2) * dist_x + d - (i/2)*1.5, start_y + (int)(i/2) * dist_y + d + (i%2)*2);
			glEnd();
		}
		else
		{
			start_x = 196.8; start_y = -50.7;
			glBegin(GL_LINES); 
			glVertex2d(start_x + (i%2) * dist_x - ((i-10)/2)*1.5, start_y + (int)((i-10)/2) * dist_y + (i%2)*2);
			glVertex2d(start_x + (i%2) * dist_x + d - ((i-10)/2)*1.5, start_y + (int)((i-10)/2) * dist_y + d + (i%2)*2);
			glEnd();
		}
	}
	//end of Right Leg
}