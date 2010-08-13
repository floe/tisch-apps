/***********************
Part of "Puzzle"
by Andreas Dippon 2010
mail: dippona@in.tum.de
************************/

#include "puzzlepiece.h"
#include "global.h"
#include "ghostcontainer.h"

puzzlepiece::puzzlepiece( int _w, int _h, int _x, int _y, double angle, RGBATexture* _tex, int mode):
	Container( _w,_h,_x,_y,angle,_tex,mode) 
{ 
	in_gc = false;
	has_blob = false;
	alpha = 1.0;
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

void puzzlepiece::action( Gesture* gesture ) 
{
	Vector tmpvec;
	if ( gesture->name() == "move" ) 
	{
			FeatureBase* f = (*gesture)[1];
			BlobCount* p = dynamic_cast<BlobCount*>(f);
			f = (*gesture)[0];
			Motion* m = dynamic_cast<Motion*>(f);
			tmpvec = m->result();
			if (p->result() == 3)
			{
				if(tmpvec.y < 0) alpha -= 0.01f;
				else alpha += 0.01f;
				if(alpha > 1.0) alpha = 1.0;
				else if(alpha < 0.2) alpha = 0.2;
				color(1,1,1,alpha);
				return;
			}
	}
	has_blob = true;

	if(gesture->name() == "release")
	{
		//has_blob = false;
		checkforsnap();
	}
	if(in_gc)
	{
		if(gesture->name() == "move" && checkforunsnap(tmpvec))
		{
			unsnap();
			Container::action(gesture);
		}
		else
			this->parent->action(gesture);
	}
	else
		Container::action(gesture);
}

void puzzlepiece::checkforsnap()
{
	this->update();
	h2 = this->h * this->asy / 2;
	w2 = this->w * this->asx / 2;
	x2 = cos(-1 * this->absangle) * ((Vector)(this->abspos)).x - sin(-1 * this->absangle) * ((Vector)(this->abspos)).y; //	x' = cos(a) * x - sin(a) * y
	y2 = sin(-1 * this->absangle) * ((Vector)(this->abspos)).x + cos(-1 * this->absangle) * ((Vector)(this->abspos)).y; //	y' = sin(a) * x + cos(a) * y

	for(std::vector<puzzlepiece*>::iterator it = puzzlepieces.begin(); it != puzzlepieces.end(); it++)
	{
		if(((*it)->x == this->x) && ((*it)->y == this->y))
			continue;
		if(this->in_gc && (*it)->in_gc && this->parent == (*it)->parent)
			continue;


		if((int)fabs((this->absangle - (*it)->absangle)*180/M_PI)%90 > 85 || (int)fabs((this->absangle - (*it)->absangle)*180/M_PI)%90 < 5)
		{
			x1 = cos(-1 * this->absangle) * ((Vector)((*it)->abspos)).x - sin(-1 * this->absangle) * ((Vector)((*it)->abspos)).y; //	x' = cos(a) * x - sin(a) * y
			y1 = sin(-1 * this->absangle) * ((Vector)((*it)->abspos)).x + cos(-1 * this->absangle) * ((Vector)((*it)->abspos)).y; //	y' = sin(a) * x + cos(a) * y
			double helpangle, help2;
			switch((int)(fabs((this->absangle - (*it)->absangle)*180/M_PI)/85))
			{
			case 0: case 2: case 4:
				h1 = (*it)->h * (*it)->asy / 2;
				w1 = (*it)->w * (*it)->asx / 2;
				break;
			case 1: case 3:
				h1 = (*it)->w * (*it)->asx / 2;
				w1 = (*it)->h * (*it)->asy / 2;
				break;
			default:
				std::cout << "angle error" << std::endl;
			}
			if((fabs(fabs(x1-x2) - (w1+w2))) < 5)//if 1 left/right of 2
				if((y1 > y2 && (y1 - h1 < y2 + h2)) || (y1 < y2 && (y1 + h1 > y2 - h2)) || y1 == y2) 
				{
					if(x2 < x1) x2 = x1 - w1 - w2;
					else x2 = x1 + w1 + w2;
					snap((*it));
				}
			if((fabs(fabs(y1-y2) - (h1+h2))) < 5) //if 1 above/below of 2
				if((x1 > x2 && (x1 - w1 < x2 + w2)) || (x1 < x2 && (x1 + w1 > x2 - w2)) || x1 == x2) 
				{
					if(y2 < y1) y2 = y1 - h1 - h2;
					else y2 = y1 + h1 + h2;
					snap((*it));
				}
		}
	}
}

void puzzlepiece::snap(puzzlepiece* piece)
{
	std::cout << "Snap!" << std::endl;
	if(!piece->in_gc)
	{
		if(this->in_gc)
		{
			//position correction
			this->parent->x = this->parent->x - this->abspos.x + cos(this->absangle) * x2 - sin(this->absangle) * y2; //	x' = cos(a) * x - sin(a) * y
			this->parent->y = this->parent->y - this->abspos.y + sin(this->absangle) * x2 + cos(this->absangle) * y2; //	y' = sin(a) * x + cos(a) * y
			
			//angle correction
			double correction = piece->absangle - this->absangle;
			while(correction > M_PI/36) correction -= M_PI/2;
			while(correction < -M_PI/36) correction += M_PI/2;
			this->parent->angle += correction;

			//scale correction of piece
			piece->sx = piece->sx / this->parent->sx;
			piece->sy = piece->sy / this->parent->sy;

			piece->angle -= this->parent->absangle;
			piece->x -= this->parent->x;
			piece->y -= this->parent->y;
			piece->x /= this->parent->sx;
			piece->y /= this->parent->sy;
			double help_x = piece->x;
			double help_y = piece->y;
			piece->x = cos(-this->parent->angle) * help_x - sin(-this->parent->angle) * help_y; //	x' = cos(a) * x - sin(a) * y
			piece->y = sin(-this->parent->angle) * help_x + cos(-this->parent->angle) * help_y; //	y' = sin(a) * x + cos(a) * y

			piece->parent->remove(piece);
			this->parent->add(piece);

			((ghostcontainer*)(this->parent))->correct_center();
			piece->in_gc = true;
		}
		else
		{
			//position correction
			this->x = this->x - this->abspos.x + cos(this->absangle) * x2 - sin(this->absangle) * y2; //	x' = cos(a) * x - sin(a) * y
			this->y = this->y - this->abspos.y + sin(this->absangle) * x2 + cos(this->absangle) * y2; //	y' = sin(a) * x + cos(a) * y

			//angle correction
			double correction = piece->absangle - this->absangle;
			while(correction > M_PI/36) correction -= M_PI/2;
			while(correction < -M_PI/36) correction += M_PI/2;
			this->angle = this->absangle + correction;

			ghostcontainer* gc = new ghostcontainer(1,1,(this->x+piece->x)/2,(this->y+piece->y)/2,0);
			this->parent->add(gc);
			piece->parent->remove(piece);
			this->parent->remove(this);
			gc->add(piece);
			gc->add(this);

			this->x -= gc->x;
			this->y -= gc->y;
			piece->x -= gc->x;
			piece->y -= gc->y;

			this->in_gc = true;
			piece->in_gc = true;
		}
	}
	else //piece->in_gc
	{
		if(this->in_gc)
		{
			//position correction
			this->parent->x = this->parent->x - this->abspos.x + cos(this->absangle) * x2 - sin(this->absangle) * y2; //	x' = cos(a) * x - sin(a) * y
			this->parent->y = this->parent->y - this->abspos.y + sin(this->absangle) * x2 + cos(this->absangle) * y2; //	y' = sin(a) * x + cos(a) * y
			
			//angle correction
			double correction = piece->absangle - this->absangle;
			while(correction > M_PI/36) correction -= M_PI/2;
			while(correction < -M_PI/36) correction += M_PI/2;
			this->parent->angle += correction;
			
			this->update();

			((ghostcontainer*)(this->parent))->move_pieces((ghostcontainer*)(piece->parent));
			((ghostcontainer*)(this->parent))->correct_center();
		}
		else
		{
			//position correction
			this->x = this->x - this->abspos.x + cos(this->absangle) * x2 - sin(this->absangle) * y2; //	x' = cos(a) * x - sin(a) * y
			this->y = this->y - this->abspos.y + sin(this->absangle) * x2 + cos(this->absangle) * y2; //	y' = sin(a) * x + cos(a) * y

			//angle correction
			double correction = piece->absangle - this->absangle;
			while(correction > M_PI/36) correction -= M_PI/2;
			while(correction < -M_PI/36) correction += M_PI/2;
			this->angle += correction;	
			
			//scale correction of this
			this->sx = this->sx / piece->parent->sx;
			this->sy = this->sy / piece->parent->sy;

			this->angle -= piece->parent->absangle;

			this->x -= piece->parent->x;
			this->y -= piece->parent->y;
			this->x /= piece->parent->sx;
			this->y /= piece->parent->sy;
			double help_x = this->x;
			double help_y = this->y;
			this->x = cos(-piece->parent->angle) * help_x - sin(-piece->parent->angle) * help_y; //	x' = cos(a) * x - sin(a) * y
			this->y = sin(-piece->parent->angle) * help_x + cos(-piece->parent->angle) * help_y; //	y' = sin(a) * x + cos(a) * y

			this->parent->remove(this);
			piece->parent->add(this);

			((ghostcontainer*)(this->parent))->correct_center();
			this->in_gc = true;
		}
	}
}

bool puzzlepiece::checkforunsnap(Vector vec)
{
	if(vec.length() < 5) return false;
	return ((ghostcontainer*)(this->parent))->check_active_widgets();
}

void puzzlepiece::unsnap()
{
	std::cout << "Unsnap!" << std::endl;
	Container* help = ((ghostcontainer*)(this->parent))->myparent();

	this->sx = this->asx / help->asx;
	this->sy = this->asy / help->asy;

	this->angle = this->absangle;

	this->x = this->abspos.x - help->abspos.x;
	this->y = this->abspos.y - help->abspos.y;
	this->x /= help->sx;
	this->y /= help->sy;
	double help_x = this->x;
	double help_y = this->y;
	this->x = cos(-this->parent->angle) * help_x - sin(-this->parent->angle) * help_y; //	x' = cos(a) * x - sin(a) * y
	this->y = sin(-this->parent->angle) * help_x + cos(-this->parent->angle) * help_y; //	y' = sin(a) * x + cos(a) * y

	ghostcontainer* old_cont = (ghostcontainer*)this->parent;
	this->parent->remove(this);
	old_cont->correct_center();
	help->add(this);
	this->in_gc = false;

	old_cont->remove_last();
}