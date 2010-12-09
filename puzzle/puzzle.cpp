/***********************
Puzzle - libTISCH demo
by Andreas Dippon 2010
mail: dippona@in.tum.de
************************/

#include "puzzlepiece.h"
#include "global.h"

std::vector<puzzlepiece*> puzzlepieces;

int main( int argc, char* argv[] ) {

	std::cout << "Puzzle - libTISCH demo" << std::endl;
	std::cout << "by Andreas Dippon (dippona@in.tum.de) 2010" << std::endl;

	int mouse = ((argc > 1) && (std::string("-m") == argv[1]));
	Window* win = new Window( 640, 480, "Puzzle", mouse );
	win->texture(0);

	srandom(45890);
	
	// load the textures
	for (int i = mouse+1; i < argc; i++) {
		RGBATexture* tmp = new RGBATexture( argv[i] );
		puzzlepiece* foo = new puzzlepiece( 
			tmp->width(1)/5, 
			tmp->height(1)/5,
			(int)(((double)random()/(double)RAND_MAX)*600-300),
			(int)(((double)random()/(double)RAND_MAX)*450-225),
			/*(int)(((double)random()/(double)RAND_MAX)*360)*/0,
			tmp, 0x07
		);
		puzzlepieces.push_back(foo);
		win->add( foo );
	}

	win->update();
	win->run();
}

