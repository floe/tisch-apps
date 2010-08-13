#ifndef _HABITAT
#define _HABITAT

class Habitat
{
public:
	virtual void update() = 0;
	virtual ~Habitat() { }

private:
	int width, height;
	int centerX, centerY;
	float down, up, left, right;
};

#endif
