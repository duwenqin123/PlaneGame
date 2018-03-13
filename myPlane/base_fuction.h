#pragma once
//#include<iostream>
#include<Windows.h>
#include<conio.h>
#include<time.h>
//#include"Game.h"

extern bool inBottom ;
//using namespace std;
struct Frame {
	COORD position[2];
};
void toPos(COORD pos);
void toPos(int x, int y);
void drawRow(int y, int x1, int x2, char ch);
void drawCol(int x, int y1, int y2, char ch);
void drawFrame(COORD a, COORD b, char ch1, char ch2);
void drawFrame(int left, int top, int right, int bottom,char ch1,char ch2);
void drawFrame(Frame frame, char ch1, char ch2);
int random(int a, int b);
COORD random(COORD a, COORD b);
void hideCursor();
void outputInPos(COORD pos,char ch	);
