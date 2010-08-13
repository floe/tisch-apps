/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "engine.h"
#include "menu.h"
#include "unit.h"
#include "gamefield.h"
#include "mainMenu.h"
#include "unitSelectionMenu.h"
#include "player.h"
#include <widgets/Window.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "unitDataContainer.h"
#include "playerMenu.h"
#include "infoMenu.h"
#include "damageResolutionMenu.h"
#include "victoryMenu.h"

int main(int argc, char **argv)
{
	srand ( (unsigned int)time ( NULL ) );
	
	Window* win = new Window( 800, 600, "BattleTech", (argc > 1) );
	win->texture( 0 );
//	win->usePeak();

	tex_storage.init();

	menu* main = new mainMenu(300,400,10,10,0.0, tex_storage.getContainerTexture(),0xFF);
	win->add(main);

	win->run();
}