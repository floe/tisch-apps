/*************************************************************************\
*    Datatransfer - libTISCH demo - see http://tisch.sourceforge.net/     *
*     Copyright (c) 2012 by Norbert Wiedermann, <wiederma@in.tum.de>      *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/
#pragma once

#include <stdlib.h>
#include <nanolibc.h>
#include "Window.h"
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <sstream>
#include "BlobMarker.h"
#include "BlobCount.h"
#include "BlobPos.h"

#include "Label.h"
#include "Motion.h"

#include "MyImage.h"

using namespace std;

class MyImage;

class HandyDropZone : public Container {
public:
	HandyDropZone( int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0x05):
	Container( _w, _h, _x, _y, 0, _tex, mode ) {
		shadow = true;
	};

	void deleteMe() {
		parent->remove(this);
		delete this;
	};

	int myMarkerID;
	std::vector<MyImage*> imageVector;
};