/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "victoryMenu.h"

victoryMenu::victoryMenu(int w, int h, int draw, int x, int y, double angle, RGBATexture* tex, int mode) : 
	menu(w, h, x, y, angle, tex, mode)
{
	mytex = tex_storage.getmenu();
	if(draw == 0)
		this->add(new Label("Draw.", 60,30, 0, h/4));
	else
	{
		for(int i = 0; i < (signed)gameengine.players.size(); i++)
		{
			if(!gameengine.players[i]->defeated)
			{
				std::string help = "Player ";
				help.append(gameengine.IntToString(i+1));
				help.append(" wins!");
				this->add(new Label(help.c_str(), w-10, 30, 0, h/4));
				break;
			}
		}
	}
	this->add(new menubutton(100, 20, 35, -w/4, -h/2 + 15));
	this->add(new Label("Play again", 80, 20, -w/4, -h/2 + 15));
	this->add(new menubutton(80, 20, 0, w/4, -h/2 + 15));
	this->add(new Label("Exit", 60, 20, w/4, -h/2 + 15));
}
