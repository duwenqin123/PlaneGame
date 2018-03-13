#include <windows.h>
#include <tchar.h>//使用swprintf_s函数所需的头文件
#include  <time.h> 
#include "Game.h"
#include "base_fuction.h"

#pragma comment(lib,"winmm.lib")			//调用PlaySound函数所需库文件
#pragma  comment(lib,"Msimg32.lib")		//添加使用TransparentBlt函数所需的库文件

#define WINDOW_WIDTH	800							
#define WINDOW_HEIGHT	600						
#define WINDOW_TITLE		L"Plane Game"	

enum DIR{LEFT,RIGHT,UP,DOWN};


HDC				g_hdc = NULL, g_mdc = NULL, g_bufdc = NULL;      //全局设备环境句柄与全局内存DC句柄
DWORD		g_tPre = 0, g_tNow = 0;					//声明l两个函数来记录时间,g_tPre记录上一次绘图的时间，g_tNow记录此次准备绘图的时间
											 //一系列位图句柄的定义
HBITMAP		g_hBackGround;
HBITMAP		g_hBombLightingBitmap,g_hBullet0Bitmap,g_hBullet1Bitmap,
g_hBonusEnhancedBulletBitmap,g_hBonusBombSkillBitmap,
g_hMyPlaneBitmap, g_hEnermy0Bitmap, g_hEnermy1Bitmap,
g_hEnermy2Bitmap, g_hMyBulletBitmap, g_hEnermyBulletBitmap;  

Game g_game;//游戏类，包含游戏中的飞机，子弹等的初始化，判断函数等

LRESULT CALLBACK	WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL						Game_Init(HWND hwnd);			//在此函数中进行资源的初始化
VOID							GameLoop(HWND hwnd);		//在此函数中进行绘图代码的书写和程序运行的核心代码
BOOL						Game_ShutDown(HWND hwnd);	//在此函数中进行资源的清理

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
	WNDCLASSEX wndClass = { 0 };							
	wndClass.cbSize = sizeof(WNDCLASSEX);			
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	
	wndClass.lpfnWndProc = WndProc;					
	wndClass.cbClsExtra = 0;								
	wndClass.cbWndExtra = 0;							
	wndClass.hInstance = hInstance;						
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);    
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  
	wndClass.lpszMenuName = NULL;						
	wndClass.lpszClassName = L"MyPlane";		

	if (!RegisterClassEx(&wndClass))				
		return -1;

	HWND hwnd = CreateWindow(L"MyPlane", WINDOW_TITLE,				
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//调整窗口显示时的位置，使窗口左上角位于（250,80）处
	ShowWindow(hwnd, nShowCmd);    
	UpdateWindow(hwnd);						

	if (!Game_Init(hwnd)){
		MessageBox(hwnd, L"资源初始化失败", L"消息窗口", 0); 
		return FALSE;
	}
	PlaySound(L"GameMedia\\战斗1-森林.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); //循环播放背景音乐 

	MSG msg = { 0 };				
	while (msg.message != WM_QUIT){
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);		//将虚拟键消息转换为字符消息
			DispatchMessage(&msg);			//分发一个消息给窗口程序。
		}
		
		else{
		g_tNow = GetTickCount();   //获取当前系统时间
		if (g_tNow - g_tPre >= 60)      //当此次循环运行与上次绘图时间相差0.06秒时再进行重绘操作
			GameLoop(hwnd);
		}
	}

	UnregisterClass(L"MyPlane", wndClass.hInstance);  //程序准备结束，注销窗口类
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	PAINTSTRUCT paintStruct;

	switch (message){
	case WM_PAINT:
		g_hdc = BeginPaint(hwnd, &paintStruct);
			GameLoop(hwnd);
		EndPaint(hwnd, &paintStruct);
		ValidateRect(hwnd, NULL);		// 更新客户区的显示
		break;
		
	case WM_KEYDOWN:					
		switch (wParam){
		case VK_ESCAPE:		//按下【Esc】键
			PostQuitMessage(0);
			break;
		case VK_RETURN:    //Enter 键，不论初始状态还是游戏结束，按enter都将开始游戏
			if (g_game.gameState !=GAME_START) {
				g_game.gameState = GAME_START;
				g_game.healthPoint = 3;
				g_game.planeSuperState = TRUE;
				g_game.bombNum = 0;
				g_game.bombIsUsed = FALSE;
			}
			break;
		case VK_LEFT:
			if(g_game.plane.pos.X>0)
				g_game.plane.pos.X -= 20;
			break;
		case VK_RIGHT:
			if(g_game.plane.pos.X<WINDOW_WIDTH-70)//70只是一个很粗略的估计，下面出现的数字也基本都是估计
				g_game.plane.pos.X += 20;
			break;
		case VK_DOWN:
			if(g_game.plane.pos.Y<WINDOW_HEIGHT-100)
				g_game.plane.pos.Y += 20;
			break;
		case VK_UP:
			if(g_game.plane.pos.Y>0)
				g_game.plane.pos.Y -= 20;
			break;
		//case VK_CONTROL://Ctrl键，发射子弹
			//g_game.shoot();
			//break;
		case VK_SPACE://空格键，炸弹
			g_game.bomb();
			break;
		default:
			break;
		}
		break;
	case WM_DESTROY:					//若是窗口销毁消息
		Game_ShutDown(hwnd);			
		PostQuitMessage(0);			
		break;					
	default:				
		return DefWindowProc(hwnd, message, wParam, lParam);		
	}

	return 0;									
}

BOOL Game_Init(HWND hwnd)
{
	srand((unsigned)time(NULL));      

	HBITMAP bmp;

	//三缓冲体系的创建
	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);  //创建一个和hdc兼容的内存DC
	g_bufdc = CreateCompatibleDC(g_hdc);//再创建一个和hdc兼容的缓冲DC
	bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT); //建一个和窗口兼容的空的位图对象

	SelectObject(g_mdc, bmp);//将空位图对象放到mdc中

							 //载入一系列游戏资源图到位图句柄中
	g_hBackGround = (HBITMAP)LoadImage(NULL, L"GameMedia\\bg.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);  //背景位图
	g_hMyPlaneBitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\plane.bmp", IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE);  //怪物角色位图
	g_hEnermy0Bitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\enermy0.bmp", IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE);  //英雄角色位图
	g_hEnermy1Bitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\enermy1.bmp", IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE);  //英雄角色位图
	g_hEnermy2Bitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\enermy2.bmp", IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE);  //英雄角色位图
	g_hMyBulletBitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\myBullet.bmp", IMAGE_BITMAP, 15, 15, LR_LOADFROMFILE);  //英雄角色位图
	g_hBullet0Bitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\bullet0.bmp", IMAGE_BITMAP, 15, 15, LR_LOADFROMFILE);  //英雄角色位图
	g_hBullet1Bitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\bullet1.bmp", IMAGE_BITMAP, 15, 15, LR_LOADFROMFILE);  //英雄角色位图
	g_hBombLightingBitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\lightning.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);  //英雄角色位图
	g_hEnermyBulletBitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\enermyBullet.bmp", IMAGE_BITMAP, 15, 15, LR_LOADFROMFILE);  //英雄角色位图
	g_hBonusEnhancedBulletBitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\bonusEnhancedBullet.bmp", IMAGE_BITMAP, 25, 25, LR_LOADFROMFILE);  //怪物角色位图
	g_hBonusBombSkillBitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\bonusBombSkill.bmp", IMAGE_BITMAP, 25, 25, LR_LOADFROMFILE);  //怪物角色位图

																															//设置字体
	HFONT hFont;
	hFont = CreateFont(25, 0, 0, 0, 700, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("微软雅黑"));
	SelectObject(g_mdc, hFont);
	SetBkMode(g_mdc, TRANSPARENT);    //设置文字显示背景透明

	GameLoop(hwnd);  //调用一次游戏Main函数
	return TRUE;
}

//-----------------------------------【GameLoop( )函数】--------------------------------------
//	描述：游戏Main函数，处理游戏大大小小函数的合理调用与运作
//--------------------------------------------------------------------------------------------------
VOID GameLoop(HWND hwnd){
	wchar_t str[30];   //用来临时存储字符串的数组

	//先在mdc中贴上背景图，并让背景循环移动
	SelectObject(g_bufdc, g_hBackGround);
	static int nYOffset = 0;
	nYOffset = (nYOffset + 3) % WINDOW_HEIGHT;//3决定了背景移动的速度
	for (int i = 0; i < 1; i++){
		for (int j = -1; j < 1; j++){
			BitBlt(g_mdc, i*WINDOW_WIDTH, j*WINDOW_HEIGHT + nYOffset, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);
		}
	}

	//显示准备阶段时的界面
	if (g_game.gameState == GAME_PREPARE) {
		SetTextColor(g_mdc, RGB(0, 255, 0));
		TextOut(g_mdc, 300, 250, L"Bullet: Ctrl", wcslen(L"Bullet: Ctrl"));
		TextOut(g_mdc, 300, 280, L"Bomb: Space", wcslen(L"Bomb: Space"));
		TextOut(g_mdc, 300, 310, L"Move: Direction Key", wcslen((L"Move: Direction Key")));
		//TextOut(g_mdc, 300, 340, L"Start: Enter", wcslen((L"Start: Enter")));
		TextOut(g_mdc, 300, 340, L"Exit: Esc", wcslen(L"Exit: Esc"));

		//显示闪烁地按Enter键的提示
		static int startCount = 0;
		if (startCount >= 8)
			TextOut(g_mdc, 300, 420, L"Press Enter to start!", wcslen(L"Press Enter to start!"));
		startCount++;
		if (startCount == 16)
			startCount = 0;
	}
	else if (g_game.gameState == GAME_START) {
		if (GetAsyncKeyState(VK_CONTROL))
			g_game.shoot();

		//设置飞机无敌模式的持续时间（估计）
		if (g_game.planeSuperState == TRUE) {
			static int g_superStateTimeCount = 0;
			if (g_superStateTimeCount == 200) {
				g_game.planeSuperState = FALSE;
				g_superStateTimeCount = 0;
			}
			g_superStateTimeCount++;
			//if (g_superStateTimeCount == 21)
				//g_superStateTimeCount = 0;
		}

		if (g_game.planeSuperState == TRUE){ //无敌时的闪烁效果
			static int g_superPlaneFlickerCount = 0;
			if (g_superPlaneFlickerCount <= 3) {
				SelectObject(g_bufdc, g_hMyPlaneBitmap);
				TransparentBlt(g_mdc, g_game.plane.pos.X, g_game.plane.pos.Y, 50, 50, g_bufdc, 0, 0, 50, 50, RGB(0, 0, 0));
			}
			g_superPlaneFlickerCount++;
			if (g_superPlaneFlickerCount == 6)
				g_superPlaneFlickerCount = 0;
		}
		else {
			SelectObject(g_bufdc, g_hMyPlaneBitmap);
			TransparentBlt(g_mdc, g_game.plane.pos.X, g_game.plane.pos.Y, 50, 50, g_bufdc, 0, 0, 50, 50, RGB(0, 0, 0));
		}

		//在界面上方显示分数，生命值等
		SetTextColor(g_mdc, RGB(255, 0, 0));
		memset(str, 0, sizeof(str));
		swprintf_s(str, L"Score:%2d", g_game.score);
		TextOut(g_mdc, 100, 0, str, wcslen(str));
		memset(str, 0, sizeof(str));
		swprintf_s(str, L"HP:%2d", g_game.healthPoint);
		TextOut(g_mdc, 200, 0, str, wcslen(str));
		memset(str, 0, sizeof(str));
		swprintf_s(str, L"Enermy shot down:%2d", g_game.enermyShot);
		TextOut(g_mdc, 280, 0, str, wcslen(str));
		memset(str, 0, sizeof(str));
		swprintf_s(str, L"Bomb:%3d", g_game.bombNum);
		TextOut(g_mdc, 500, 0, str, wcslen(str));

		//显示敌机，当使用了炸弹时，清除飞机旁的敌机，并重新随机生成敌机
		for (int i = 0; i < ENERMY_NUM; i++) {
			if (!g_game.bombIsUsed || g_game.enermyGroup[i]->pos.Y >= g_game.plane.pos.Y + 200 || g_game.enermyGroup[i]->pos.Y <= g_game.plane.pos.Y - 200) {
				if (g_game.enermyGroup[i]->type == ENERMY0) {
					SelectObject(g_bufdc, g_hEnermy0Bitmap);
					TransparentBlt(g_mdc, g_game.enermyGroup[i]->pos.X, g_game.enermyGroup[i]->pos.Y, 50, 50, g_bufdc, 0, 0, 50, 50, RGB(0, 0, 0));
				}
				else if (g_game.enermyGroup[i]->type == ENERMY1) {
					SelectObject(g_bufdc, g_hEnermy1Bitmap);
					TransparentBlt(g_mdc, g_game.enermyGroup[i]->pos.X, g_game.enermyGroup[i]->pos.Y, 50, 50, g_bufdc, 0, 0, 50, 50, RGB(0, 0, 0));
				}
				else {
					SelectObject(g_bufdc, g_hEnermy2Bitmap);
					TransparentBlt(g_mdc, g_game.enermyGroup[i]->pos.X, g_game.enermyGroup[i]->pos.Y, 50, 50, g_bufdc, 0, 0, 50, 50, RGB(0, 0, 0));
				}
			}
			else {
				COORD temp;
				temp.X = WINDOW_WIDTH - 60;
				temp.Y = WINDOW_HEIGHT / 2-150;
				delete g_game.enermyGroup[i];
				EnermyType type = (EnermyType)random(0, 3);
				switch(type){
				case ENERMY0:
					g_game.enermyGroup[i] = new Enermy(random({ 0,0 }, temp));
					g_game.enermyGroup[i]->type = ENERMY0;
					break;
				case ENERMY1:
					g_game.enermyGroup[i] = new Enermy1(random({ 0,0 }, temp));
					g_game.enermyGroup[i]->type = ENERMY1;
					break;
				case ENERMY2:
					g_game.enermyGroup[i] = new Enermy2(random({ 0,0 }, temp));
					g_game.enermyGroup[i]->type = ENERMY2;
					break;
				default:
					break;
				}
			}
		}

		SelectObject(g_bufdc, g_hMyBulletBitmap);
		for (int i = 0; i < BULLET_NUM; i++)
			if (g_game.bulletGroup[i].flag) {
				//贴上我方飞机子弹图
				TransparentBlt(g_mdc, g_game.bulletGroup[i].pos.X, g_game.bulletGroup[i].pos.Y, 15, 15, g_bufdc, 0, 0, 15, 15, RGB(0, 0, 0));
				//移动子弹，并判断是否出界
				g_game.bulletGroup[i].pos.Y -= 30;
				if (g_game.bulletGroup[i].pos.Y < 0) {
					g_game.bulletGroup[i].flag = false;
				}
			}

		if (g_game.plane.skill & ENHANCED_BULLET) {
			for (int i = 0; i < BULLET_NUM; i++) {
				if (g_game.enhancedBullet0[i].flag) {
					//贴上加成子弹图
					SelectObject(g_bufdc, g_hBullet0Bitmap);
					TransparentBlt(g_mdc, g_game.enhancedBullet0[i].pos.X, g_game.enhancedBullet0[i].pos.Y, 15, 15, g_bufdc, 0, 0, 15, 15, RGB(0, 0, 0));
					g_game.enhancedBullet0[i].pos.X -= 25;
					g_game.enhancedBullet0[i].pos.Y -= 25;
					if (g_game.enhancedBullet0[i].pos.X < 0 || g_game.enhancedBullet0[i].pos.Y < 0)
						g_game.enhancedBullet0[i].flag = false;
				}
				if (g_game.enhancedBullet1[i].flag) {

					SelectObject(g_bufdc, g_hBullet1Bitmap);
					TransparentBlt(g_mdc, g_game.enhancedBullet1[i].pos.X, g_game.enhancedBullet1[i].pos.Y, 15, 15, g_bufdc, 0, 0, 15, 15, RGB(0, 0, 0));
					g_game.enhancedBullet1[i].pos.X += 25;
					g_game.enhancedBullet1[i].pos.Y -= 25;
					if (g_game.enhancedBullet1[i].pos.X > WINDOW_WIDTH - 20 || g_game.enhancedBullet1[i].pos.Y < 0)
						g_game.enhancedBullet1[i].flag = false;
				}
			}
		}
		
		//设定加成子弹持续时间（估计）
		if (g_game.plane.skill & ENHANCED_BULLET) {
			static	int g_EnhancedBulletCount = 0;
			if (g_EnhancedBulletCount == 30) {
				g_EnhancedBulletCount = 0;
				g_game.plane.skill &= ~ENHANCED_BULLET;
				g_game.closeEnhancedBullet();
			}
			g_EnhancedBulletCount++;
		}
		
		//随机生成bonus
		if (random(0, 5) == 0) {
			int type = random(0, 2);
			switch (type) {
			case 0:
				g_game.addBonus(ENHANCED_BULLET);
				break;
			case 1:
				g_game.addBonus(BOMB_SKILL);
				break;
			default:
				break;
			}
		}

		//贴上bonus图并移动bonus和判断是否出界
		for (int i = 0; i < BONUS_NUM; i++)
			if (g_game.bonusGroup[i].flag) {
				switch (g_game.bonusGroup[i].type){
				case ENHANCED_BULLET:
					SelectObject(g_bufdc, g_hBonusEnhancedBulletBitmap);
					break;
				case BOMB_SKILL:
					SelectObject(g_bufdc, g_hBonusBombSkillBitmap);
					break;
				default:
					break;
				}
				TransparentBlt(g_mdc, g_game.bonusGroup[i].pos.X, g_game.bonusGroup[i].pos.Y, 25, 25, g_bufdc, 0, 0, 25, 25, RGB(0, 0, 0));
				//随机移动bonus方向
				DIR dirX = (DIR)random(0, 4), dirY = (DIR)random(0, 4);
				switch (dirX) {
				case LEFT:
					g_game.bonusGroup[i].pos.X -= 10;
					break;
				case RIGHT:
					g_game.bonusGroup[i].pos.X += 10;
					break;
				case UP:
				case DOWN:
					break;
				}
				switch (dirY) {
				case DOWN:
					g_game.bonusGroup[i].pos.Y -= 10;
					break;
				case UP:
					g_game.bonusGroup[i].pos.Y += 10;
					break;
				case LEFT:
				case RIGHT:
					break;
				}
				//g_game.bonusGroup[i].pos.Y += 10;
				if (g_game.bonusGroup[i].pos.Y >WINDOW_HEIGHT - 50 ||
					g_game.bonusGroup[i].pos.Y<20||g_game.bonusGroup[i].pos.X<20||
					g_game.bonusGroup[i].pos.X>WINDOW_WIDTH-50) {
					g_game.bonusGroup[i].flag = false;
				}
			}

		g_game.judgeEatBonus();//判断是否吃到bonus
		g_game.judgeEnermyIsShoot();

		g_game.enermyMove();

		g_game.judgeCollide();

		g_game.judgePlaneIsShoot();

		//当爆炸时，清除我方飞机附近的子弹
		SelectObject(g_bufdc, g_hEnermyBulletBitmap);
		for (int i = 0; i < ENERMY_NUM; i++) {
			for (int j = 0; j < BULLET_NUM; j++) {
				if (!g_game.bombIsUsed ||  g_game.enermyGroup[i]->bulletGroup[j].pos.Y >= g_game.plane.pos.Y + 200 || g_game.enermyGroup[i]->bulletGroup[j].pos.Y <= g_game.plane.pos.Y - 200) {
					if (g_game.enermyGroup[i]->bulletGroup[j].flag) {
						//贴上敌机子弹图
						TransparentBlt(g_mdc, g_game.enermyGroup[i]->bulletGroup[j].pos.X, g_game.enermyGroup[i]->bulletGroup[j].pos.Y, 15, 15, g_bufdc, 0, 0, 15, 15, RGB(0, 0, 0));

						g_game.enermyGroup[i]->bulletGroup[j].pos.Y += 30;
						if (g_game.enermyGroup[i]->bulletGroup[j].pos.Y >WINDOW_HEIGHT - 20) {
							g_game.enermyGroup[i]->bulletGroup[j].flag = false;
						}
					}
				}
				else {
					g_game.enermyGroup[i]->bulletGroup[j].flag = false;
				}
			}
		}

		//显示使用炸弹后的闪光图片，并持续一定时间
		if (g_game.bombIsUsed) {
			static int bombDurationCount = 0;
			static int bombLightingFlickCount = 0;
			if (bombLightingFlickCount <= 1) {
				SelectObject(g_bufdc, g_hBombLightingBitmap);
				TransparentBlt(g_mdc, g_game.plane.pos.X - 200, g_game.plane.pos.Y - 300, 800, 600, g_bufdc, 0, 0, 800, 600, RGB(0, 0, 0));
			}
			if (bombLightingFlickCount == 4)
				bombLightingFlickCount = 0;
			if (bombDurationCount == 8) {
				bombDurationCount = 0;
				g_game.bombIsUsed = FALSE;
			}
			bombDurationCount++;
			bombLightingFlickCount++;
		}
	}

	//显示游戏结束后的界面
	else {
		SetTextColor(g_mdc, RGB(0, 255, 0));
		TextOut(g_mdc, 300, 250, L"Game   Over !", wcslen(L"Game   Over !"));
		memset(str, 0, sizeof(str));
		swprintf_s(str, L"Shoot down enermy planes: %d", g_game.enermyShot);
		TextOut(g_mdc, 300, 280, str, wcslen(str));
		memset(str, 0, sizeof(str));
		swprintf_s(str, L"Score: %d", g_game.score);
		TextOut(g_mdc, 300, 300, str, wcslen(str));

		//显示闪烁的提示
		static int enterCount = 0;
		if (enterCount >=8)
			TextOut(g_mdc, 300, 350, L"Press Enter to continue!", wcslen(L"Press Enter to continue!"));
		enterCount++;
		if (enterCount == 16)
			enterCount = 0;
		
	}

	//将mdc中的全部内容贴到hdc中
	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);
	g_tPre = GetTickCount();     //记录此次绘图时间
}
BOOL Game_ShutDown(HWND hwnd)
{
	//释放资源对象
	DeleteObject(g_hBackGround);
	DeleteObject(g_hBombLightingBitmap);
	DeleteObject(g_hBonusBombSkillBitmap);
	DeleteObject(g_hBonusEnhancedBulletBitmap);
	DeleteObject(g_hBullet0Bitmap);
	DeleteObject(g_hBullet1Bitmap);
	DeleteObject(g_hEnermyBulletBitmap);
	DeleteObject(g_hMyBulletBitmap);
	DeleteObject(g_hEnermy0Bitmap);
	DeleteObject(g_hEnermy1Bitmap);
	DeleteObject(g_hEnermy2Bitmap);
	DeleteObject(g_hMyPlaneBitmap);
	DeleteDC(g_bufdc);
	DeleteDC(g_mdc);
	ReleaseDC(hwnd, g_hdc);
	return TRUE;
}