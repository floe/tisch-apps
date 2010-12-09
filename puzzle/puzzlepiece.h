/***********************
Part of "Puzzle"
by Andreas Dippon 2010
mail: dippona@in.tum.de
************************/

#ifndef PUZZLEPIECE_H
#define PUZZLEPIECE_H

#include <stdlib.h>
#include <nanolibc.h>

#include "Window.h"
#include "Tile.h"
#include "Textbox.h"
#include <Motion.h>
#include <BlobCount.h>

class puzzlepiece: public Container
{
public:
	puzzlepiece( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0x07);

	void action(Gesture* gesture);
	void checkforsnap();
	void snap(puzzlepiece* piece);
	bool checkforunsnap(Vector vec);
	void unsnap();

	double alpha;
	bool in_gc;
	bool has_blob;
	double h1, h2, w1, w2, x1, x2, y1, y2;
};

#endif
