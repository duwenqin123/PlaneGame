#pragma once
#include"Enermy.h"
#include"base_fuction.h"
class Enermy2:public Enermy
{
public:
	//Enermy2();
	Enermy2(COORD p = { 0 ,0}, int s = 0,int mf=0);
	~Enermy2();
	void move();
	void moveVertical();
	void moveHorizonal();
};

