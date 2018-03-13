#include "Enermy1.h"



/*Enermy1::Enermy1():
{
}*/
Enermy1::Enermy1(COORD p, int s, int mf) :Enermy(p, s, mf) {
}


Enermy1::~Enermy1()
{
}
//void Enermy1::enermyHorizonalMove() {


//}
void Enermy1::move() {
	if (moveFlag==0) {
		if (pos.X <= 10) {
			moveFlag = 1;
		}
		else {
			pos.X-=10;
		}
	}
	else {
		if (pos.X >=WINDOW_WIDTH-60) {
			moveFlag = 0;
		}
		else
			pos.X+=10;
	}
	if (random(0, 5) == 0)
		shoot();
}
