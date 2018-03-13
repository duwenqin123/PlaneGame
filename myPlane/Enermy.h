#pragma once
#include<Windows.h>
#include"Bullet.h"
#include"base_fuction.h"

#define WINDOW_WIDTH 800
#define  WINDOW_HEIGHT 600

enum EnermyType{ENERMY0,ENERMY1,ENERMY2};

class Enermy
{
public:
	Enermy(COORD p = { 0 ,0}, int s = 0,int mf=0);
	~Enermy();
	Bullet bulletGroup[BULLET_NUM];
	void shoot();
	void moveVertical();
	void moveHorizonal();
	virtual void move();
	COORD pos;
	int speed;
	int moveFlag;
	EnermyType type;
};

