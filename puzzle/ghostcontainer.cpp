/***********************
Part of "Puzzle"
by Andreas Dippon 2010
mail: dippona@in.tum.de
************************/

#include "ghostcontainer.h"


ghostcontainer::ghostcontainer( int _w, int _h, int _x, int _y, double angle, RGBATexture* _tex, int mode):
	Container( _w,_h,_x,_y,angle,_tex,mode) 
{ 
	this->color(0,0,0,0);
	for(std::vector<Gesture>::iterator it = region.gestures.begin(); it != region.gestures.end(); it++)
	{
		if(it->name() == "move")
		{
			region.gestures.erase(it);
			break;
		}
	}

	Gesture move( "move" , GESTURE_FLAGS_DEFAULT|GESTURE_FLAGS_STICKY);

	move.push_back(new Motion());
	std::vector<int> b2;
	b2.push_back( 0 );
	b2.push_back( 10000 );
	BlobCount* bcnt = new BlobCount(1<<INPUT_TYPE_FINGER);
	bcnt->bounds( b2 );
	move.push_back( bcnt );
	region.gestures.push_back( move );	
}

void ghostcontainer::outline()
{
	glOutline2d(0.0,0.0);
	glOutline2d(0.0,0.0);
}

void ghostcontainer::correct_center()
{
	if(this->widgets.size() == 0)
		return;

	double old_x = this->x;
	double old_y = this->y;

	double abspos_x_new = 0;
	double abspos_y_new = 0;
	for(std::deque<Widget*>::iterator it = this->widgets.begin(); it != this->widgets.end(); it++)
	{
		abspos_x_new += (*it)->abspos.x;
		abspos_y_new += (*it)->abspos.y;
	}
	abspos_x_new /= this->widgets.size();
	abspos_y_new /= this->widgets.size();

	this->x = this->x - this->abspos.x + abspos_x_new;
	this->y = this->y - this->abspos.y + abspos_y_new;
	double help_x = (this->x - old_x) / this->asx;
	double help_y = (this->y - old_y) / this->asy;
	double rot_x = cos(-this->absangle) * help_x - sin(-this->absangle) * help_y; //	x' = cos(a) * x - sin(a) * y
	double rot_y = sin(-this->absangle) * help_x + cos(-this->absangle) * help_y; //	y' = sin(a) * x + cos(a) * y

	for(std::deque<Widget*>::iterator it = this->widgets.begin(); it != this->widgets.end(); it++)
	{
		(*it)->x -= rot_x*this->parent->asx;
		(*it)->y -= rot_y*this->parent->asy;
	}
}

void ghostcontainer::move_pieces(ghostcontainer* gc)
{
	Widget* tmp;
	double help_x, help_y;
	while(this->widgets.begin() != this->widgets.end())
	{
		tmp = *this->widgets.begin();

		tmp->sx = tmp->asx / gc->asx;
		tmp->sy = tmp->asy / gc->asy;

		tmp->angle = tmp->absangle - gc->absangle;

		tmp->x = tmp->abspos.x - gc->abspos.x;
		tmp->y = tmp->abspos.y - gc->abspos.y;
		tmp->x /= gc->asx;
		tmp->y /= gc->asy;
		help_x = tmp->x;
		help_y = tmp->y;
		tmp->x = cos(-gc->angle) * help_x - sin(-gc->angle) * help_y; //	x' = cos(a) * x - sin(a) * y
		tmp->y = sin(-gc->angle) * help_x + cos(-gc->angle) * help_y; //	y' = sin(a) * x + cos(a) * y
		
		this->widgets.erase(this->widgets.begin());
		gc->add(tmp);
	}
	this->parent->remove(this);
	delete this;
}

bool ghostcontainer::check_active_widgets()
{
	int counter = 0;
	for(std::deque<Widget*>::iterator it = this->widgets.begin(); it != this->widgets.end(); it++)
	{
		counter += ((puzzlepiece*)(*it))->has_blob;
	}
	if(counter > 1) return true;
	return false;
}

Container* ghostcontainer::myparent()
{
	return this->parent;
}


void ghostcontainer::remove_last()
{
	if(this->widgets.size() == 1)
	{
		((puzzlepiece*)(*this->widgets.begin()))->unsnap();
	}
}