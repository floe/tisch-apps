/***********************
Part of "Puzzle"
by Andreas Dippon 2010
mail: dippona@in.tum.de
************************/

#ifndef GHOSTCONTAINER_H
#define GHOSTCONTAINER_H

#include <stdlib.h>
#include <nanolibc.h>

#include "Window.h"
#include "Tile.h"
#include "Textbox.h"
#include <Motion.h>
#include <BlobCount.h>
#include "puzzlepiece.h"

class ghostcontainer: public Container
{
public:
	ghostcontainer( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0xFF);

	void outline();
	void correct_center();
	void move_pieces(ghostcontainer* gc);
	bool check_active_widgets(); //returns true if 2 or more widgets have blobs
	Container* myparent();
	void remove_last();
};

#endif