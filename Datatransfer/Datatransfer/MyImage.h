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

#include "HandyDropZone.h"

using namespace std;

class HandyDropZone;

class MyImage : public Container {
public: MyImage(int _w, int _h, int _x = 0, int _y = 0, double angle = 0.0, RGBATexture* _tex = 0, int mode = 0xFF):
    Container( _w, _h, _x, _y, angle, _tex, mode) {
	};

	virtual void setImage(char* imgName);
	virtual void action( Gesture* gesture);
	virtual void draw();
	void deleteMe() {
		parent->remove(this);
		//delete this;
	};

	void removeHDZ(int markerID) {
		// find the pointer to HDZ in vector and remove it
		//cout << "find pointer to HDZ, size: " << belongsToHDZ.size() << endl;
		for (std::vector<HandyDropZone*>::iterator iter = belongsToHDZ.begin(); iter != belongsToHDZ.end(); iter++) {
			//cout << "HDZ markerID: " << (*(iter))->myMarkerID << endl;
			//cout << "markerID: " << markerID << endl;
			if ((*(iter))->myMarkerID == markerID) {
				iter = belongsToHDZ.erase(iter);
				break;
			}
		}

		//cout << "new size: " << belongsToHDZ.size() << endl;
		if(belongsToHDZ.size() == 0) {
			cout << "no reference left, delete myImage" << endl;
			// no other HZD is refering to this image
			delete JPG_data;	// delete jpg data
			deleteMe();			// delete this
		}
	};
	
	std::vector<HandyDropZone*> belongsToHDZ;
	unsigned char* JPG_data;
	int JPG_data_size;
protected:
	char* imageName;
	
};