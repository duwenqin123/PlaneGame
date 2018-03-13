#pragma once
#include<windows.h>
enum BonusType{ENHANCED_BULLET=1,BOMB_SKILL=2};
class Bonus
{
public:
	Bonus();
	~Bonus();
	COORD pos;
	bool flag;
	BonusType type;
};

