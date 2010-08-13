#include "Habitat.h"

class SquareHabitat : public Habitat
{
public:
	SquareHabitat(int w=2, int h=2, int x=0, int y=0);

	void update();

	void setHeight(int h);
	void setWidth(int w);
	void setCenter(int x, int y);
	int getHeight() const; int getWidth() const;
	int getCenterX() const; int getCenterY() const;
	float getUp() const; float getDown() const;
	float getLeft() const; float getRight() const;

private:
	int width, height;
	int centerX, centerY;
	float down, up, left, right;
};

inline int SquareHabitat::getHeight() const { return height; }
inline int SquareHabitat::getWidth() const { return width; }
inline int SquareHabitat::getCenterX() const { return centerX; }
inline int SquareHabitat::getCenterY() const { return centerY; }
inline float SquareHabitat::getUp() const { return up; }
inline float SquareHabitat::getDown() const { return down; }
inline float SquareHabitat::getLeft() const { return left; }
inline float SquareHabitat::getRight() const { return right; }

