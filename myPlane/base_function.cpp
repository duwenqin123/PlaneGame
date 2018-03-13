#include<iostream>
using namespace std;
#include"base_fuction.h"
bool inBottom = false;
int random(int a, int b) {
	return (rand() % (b - a) + a);
}
COORD random(COORD a, COORD b) {
	int x = random(a.X + 1, b.X);
	int y = random(a.Y + 1, b.Y);
	COORD c = { x,y };
	return c;
}
