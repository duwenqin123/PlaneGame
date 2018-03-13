#pragma once
#include<Windows.h>
#define BULLET_NUM 10
class Bullet
{
public:
	Bullet();
	~Bullet();

	COORD pos;
	bool flag;

};

