#include "Enermy.h"

Enermy::Enermy(COORD p,int s,int mf)
{
	pos = p;
	speed = s;
	//flag = f;
	moveFlag = mf;
	for (int i = 0; i < BULLET_NUM; i++)
		bulletGroup[i].flag = false;
}

Enermy::~Enermy()
{
}

void Enermy::shoot() {	
	for (int i = 0; i < BULLET_NUM; i++) {
		if (!bulletGroup[i].flag) {
			bulletGroup[i].flag = true;
			bulletGroup[i].pos.X = pos.X+18;
			bulletGroup[i].pos.Y = pos.Y+50 ;
			break;
		}
	}
}

void Enermy::moveVertical() {
	if (pos.Y < WINDOW_HEIGHT-60) {
		pos.Y+=10;
		if (random(0, 5) == 0)
			shoot();
	}
	else
		inBottom = true;
	//Sleep(100);
}
void Enermy::move() {
	if (pos.Y < WINDOW_HEIGHT-60) {
		pos.Y+=10;
		if (random(0, 5) == 0)
			shoot();
	}
	else
		inBottom = true;
	//Sleep(100);
}
void Enermy::moveHorizonal() {
	if (moveFlag==0) {
		if (pos.X <=10) {
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