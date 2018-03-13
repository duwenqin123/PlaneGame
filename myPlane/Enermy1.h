#pragma once
#include"Enermy.h"
#include<Windows.h>
class Enermy1:public Enermy
{
public:
	//Enermy1();
	Enermy1(COORD p = { 0 ,0}, int s = 0,int mf=0);
	~Enermy1();
	void move();
	//void enermyHorizonalMove();
};

