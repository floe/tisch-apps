/***********************
Part of "Battletech"
by Andreas Dippon 2009
mail: dippona@in.tum.de
************************/

#include "warningMenu.h"

warningMenu::warningMenu(int w, int h, int x, int y, double angle, RGBATexture* tex, int mode) : 
	menu(w, h, x, y, angle, tex, mode)
{
	mytex = tex_storage.getmenu();
	myLabel = new Label("",w - 10, 30,-10,20);
	this->add(myLabel);
	this->add(new menubutton(80,20,19,0,-h/2+15));
	this->add(new Label("OK",30,20,0,-h/2+15));
}

void warningMenu::setText(std::string str)
{
	myLabel->set(str);
}

/*void warningMenu::outline()
{
	glOutline2d( -10000, -10000 );
	glOutline2d(  10000, -10000 );
	glOutline2d(  10000,  10000 );
	glOutline2d( -10000,  10000 );
}*/

void warningMenu::draw()
{
	Container::draw();
	enter();
	glColor4f(1,0,0,1);
	glLineWidth(3);
	glBegin(GL_LINE_LOOP);
		glVertex2f(-w/2, -h/2);
		glVertex2f( w/2, -h/2);
		glVertex2f( w/2,  h/2);
		glVertex2f(-w/2,  h/2);
	glEnd();
	leave();
}
