#include "Game.h"
#include<iostream>
using namespace std;

Game::Game()
{
	score = 0;
	//healthPoint = 1;
	enermyShot = 0;
	bombNum = 0;
	gameState = GAME_PREPARE;
	planeSuperState = true;
	bombIsUsed = false;
	COORD temp;
	temp.X = WINDOW_WIDTH-60;
	temp.Y = WINDOW_HEIGHT / 2;
	//随机产生敌机
	for (int i = 0; i < ENERMY_NUM; i++) {
		int c = random(0, 3);
		switch (c) {
		case 0:
			enermyGroup[i] = new Enermy(random({ 0,0 }, temp)); 
				enermyGroup[i]->type = ENERMY0;
			break;
		case 1:
			enermyGroup[i] = new Enermy1(random({ 0,0 }, temp));
				enermyGroup[i]->type = ENERMY1;
			break;
		case 2:
			enermyGroup[i] = new Enermy2(random({ 0,0 }, temp));
				enermyGroup[i]->type = ENERMY2;
			break;
		default:
			break;
		}
	}

	initBullet();
	initBonus();
	
}


Game::~Game()
{
}
void Game::judgeCollide() {
	if (!planeSuperState) {
		for (int i = 0; i < ENERMY_NUM; i++) {
			if (enermyGroup[i]->pos.Y + 30 >= plane.pos.Y&&enermyGroup[i]->pos.Y <= plane.pos.Y&& enermyGroup[i]->pos.X - 30 <= plane.pos.X&&enermyGroup[i]->pos.X + 30 >= plane.pos.X) {
				healthPoint--;
				if (healthPoint < 0) {
					gameState = GAME_OVER;
				}
				else {
					plane.pos.X = WINDOW_WIDTH / 2 - 20;
					plane.pos.Y = WINDOW_HEIGHT - 100;
					planeSuperState = true;
				}
			}
		}
	}
}

void Game::judgeEatBonus() {
	for (int i = 0; i < BONUS_NUM; i++) {
		if (plane.pos.X  <= bonusGroup[i].pos.X&&plane.pos.X + 35 >= bonusGroup[i].pos.X&&plane.pos.Y <= bonusGroup[i].pos.Y  && plane.pos.Y + 50 >= bonusGroup[i].pos.Y + 15) {
			plane.skill |= bonusGroup[i].type;
			if (plane.skill & BOMB_SKILL) {
				bombNum++;
			}
			bonusGroup[i].flag = false;
		}
	}
}
void Game::enermyMove() {
	for (int i = 0; i < ENERMY_NUM; i++) {
		enermyGroup[i]->move();

		//若敌机到达底部，重新随机生成敌机
		if (inBottom) {
			delete enermyGroup[i];
			COORD temp;
			temp.X = WINDOW_WIDTH-60;
			temp.Y = WINDOW_HEIGHT/2;
			int type = random(0, 3);
			switch (type) {
			case 0:
				enermyGroup[i] = new Enermy(random({ 0,0 }, temp));
				enermyGroup[i]->type = ENERMY0;
				break;
			case 1:
				enermyGroup[i] = new Enermy1(random({ 0,0 }, temp));
				enermyGroup[i]->type = ENERMY1;
				break;
			case 2:
				enermyGroup[i] = new Enermy2(random({ 0,0 }, temp));
				enermyGroup[i]->type = ENERMY2;
				break;
			default:
				break;
			}
			enermyGroup[i]->shoot();
			inBottom = false;
		}
	}
	Sleep(50);
}

void Game::initBullet() {
	for (int i = 0; i < BULLET_NUM; i++) {
		bulletGroup[i].flag = false;
		enhancedBullet0[i].flag = false;
		enhancedBullet1[i].flag = false;
	}
}

void Game::initBonus() {
	for (int i = 0; i < BONUS_NUM; i++) {
		bonusGroup[i].flag = false;
	}
}

void Game::shoot() {
	for (int i = 0; i < BULLET_NUM; i++) {
		if (!bulletGroup[i].flag) {
			bulletGroup[i].flag = true;
			bulletGroup[i].pos.X = plane.pos.X+18;
			bulletGroup[i].pos.Y = plane.pos.Y - 10;
			break;
		}
	}
	//初始化加成子弹
	if (plane.skill & ENHANCED_BULLET) {
		for (int i = 0; i < BULLET_NUM; i++) {
			if (!enhancedBullet0[i].flag) {
				enhancedBullet0[i].flag = true;
				enhancedBullet0[i].pos.X = plane.pos.X;
				enhancedBullet0[i].pos.Y = plane.pos.Y - 20;
				enhancedBullet1[i].flag = true;
				enhancedBullet1[i].pos.X = plane.pos.X+40;
				enhancedBullet1[i].pos.Y = plane.pos.Y -20;
				break;
			}
		}
	}
}

void  Game::bomb() {
	if (plane.skill & BOMB_SKILL) {
		bombIsUsed = true;
		bombNum--;
		if (bombNum <= 0) {
			plane.skill &= ~BOMB_SKILL;
		}
	}
}

void Game::closeEnhancedBullet() {
	for (int i = 0; i < BULLET_NUM; i++) {
		enhancedBullet0[i].flag = false;
		enhancedBullet1[i].flag = false;
	}
}

void Game::addBonus(BonusType type) {
	for (int i = 0; i < BONUS_NUM; i++) {
		if (!bonusGroup[i].flag) {
			bonusGroup[i].flag = true;
			bonusGroup[i].pos = random({ 0,0 }, { WINDOW_WIDTH - 100,WINDOW_HEIGHT / 2 + 100 });
			bonusGroup[i].type = type;
			break;
		}
	}
}

void Game::judgeEnermyIsShoot() {
	for (int i = 0; i < ENERMY_NUM; i++) {
		for (int j = 0; j < BULLET_NUM; j++) {
			if (((enermyGroup[i]->pos.X+15 <= bulletGroup[j].pos.X&&enermyGroup[i]->pos.X +35>= bulletGroup[j].pos.X)
				&& (enermyGroup[i]->pos.Y <= bulletGroup[j].pos.Y&&enermyGroup[i]->pos.Y +50>= bulletGroup[j].pos.Y)
				)||((enermyGroup[i]->pos.X + 15 <= enhancedBullet0[j].pos.X&&enermyGroup[i]->pos.X + 35 >= enhancedBullet0[j].pos.X)
				&& (enermyGroup[i]->pos.Y <= enhancedBullet0[j].pos.Y&&enermyGroup[i]->pos.Y + 50 >= enhancedBullet0[j].pos.Y) )
				|| ((enermyGroup[i]->pos.X + 15 <= enhancedBullet1[j].pos.X&&enermyGroup[i]->pos.X + 35 >= enhancedBullet1[j].pos.X)
					&& (enermyGroup[i]->pos.Y <= enhancedBullet1[j].pos.Y&&enermyGroup[i]->pos.Y + 50 >= enhancedBullet1[j].pos.Y))){
				score += 5;
				enermyShot++;
				bulletGroup[j].flag = false;
				COORD temp;
				temp.X = WINDOW_WIDTH-60;
				temp.Y = WINDOW_HEIGHT / 2;
				delete enermyGroup[i];
				int type = random(0, 3);
				switch (type) {
				case 0:
					enermyGroup[i] = new Enermy(random({ 0,0 }, temp));
					enermyGroup[i]->type = ENERMY0;
					break;
				case 1:
					enermyGroup[i] = new Enermy1(random({ 0,0 }, temp));
					enermyGroup[i]->type = ENERMY1;
					break;
				case 2:
					enermyGroup[i] = new Enermy2(random({ 0,0 }, temp));
					enermyGroup[i]->type = ENERMY2;
					break;
				default:
					break;
				}
			}
		}
	}
}

void Game::judgePlaneIsShoot() {
	if (!planeSuperState) {
		for (int i = 0; i < ENERMY_NUM; i++) {
			for (int j = 0; j < BULLET_NUM; j++) {
				if (plane.pos.X + 15 <= enermyGroup[i]->bulletGroup[j].pos.X&&plane.pos.X + 35 >= enermyGroup[i]->bulletGroup[j].pos.X&&plane.pos.Y <= enermyGroup[i]->bulletGroup[j].pos.Y + 15 && plane.pos.Y + 50 >= enermyGroup[i]->bulletGroup[j].pos.Y + 15) {
					healthPoint--;
					enermyGroup[i]->bulletGroup[j].flag = false;
					if (healthPoint < 0) {
						gameState = GAME_OVER;
					}
					else {
						plane.pos.X = WINDOW_WIDTH / 2 - 20;
						plane.pos.Y = WINDOW_HEIGHT - 100;
						planeSuperState = true;
					}
				}
			}
		}
	}
}