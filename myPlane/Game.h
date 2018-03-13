#pragma once
#include<Windows.h>
#include<iostream>
#include "Plane.h"
#include"base_fuction.h"
#include"Enermy.h"
using namespace std;
#define ENERMY_NUM 8
#define BULLET_NUM 10
#define BONUS_NUM 8
#include"Bullet.h"
#include"Enermy1.h"
#include"Enermy2.h"
#include"Bonus.h"

#define  WINDOW_WIDTH 800
#define  WINDOW_HEIGHT 600

enum State { GAME_PREPARE, GAME_START, GAME_OVER };
class Game
{
public:
	Game();
	~Game();
	State gameState;
	Plane plane;
	int bombNum;
	BOOL bombIsUsed;
	Enermy* enermyGroup[ENERMY_NUM];
	Bullet bulletGroup[BULLET_NUM];
	Bullet enhancedBullet0[BULLET_NUM], enhancedBullet1[BULLET_NUM];
	Bonus bonusGroup[BONUS_NUM];
	void shoot();
	void bomb();
	void closeEnhancedBullet();
	void judgeEnermyIsShoot();
	Bullet enermyBulletGroup[ENERMY_NUM];
	int score;
	int healthPoint;
	int enermyShot;
	bool planeSuperState;
	void judgeCollide();
	void enermyMove();
	void initBullet();
	void initBonus();
	void addBonus(BonusType type);
	void judgeEatBonus();
	void judgePlaneIsShoot();
};



