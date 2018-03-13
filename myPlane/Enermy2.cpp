#include "Enermy2.h"



/*Enermy2::Enermy2()
{
}*/
Enermy2::Enermy2(COORD p, int s, int mf) :Enermy(p, s, mf) {

}


Enermy2::~Enermy2()
{
}
void Enermy2::moveVertical() {
	if (pos.Y <=WINDOW_HEIGHT-60 ) {
		pos.Y+=10;
		if (random(0, 5) == 0)
			shoot();
	}
	else
		inBottom = true;
}
void Enermy2::moveHorizonal() {
	if (moveFlag==0) {
		if (pos.X <= 10) {
			moveFlag = 1;
		}
		else {
			pos.X-=10;
		}
	}
	else {
		if (pos.X >= WINDOW_WIDTH - 60) {
			moveFlag = 0;
		}
		else
			pos.X += 10;
	}
	if (random(0, 5) == 0)
		shoot();
}
void Enermy2::move() {
	if (random(0, 2) == 0)
		moveHorizonal();
	else
		moveVertical();
}