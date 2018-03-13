#include <windows.h>
#include <tchar.h>//ʹ��swprintf_s���������ͷ�ļ�
#include  <time.h> 
#include "Game.h"
#include "base_fuction.h"

#pragma comment(lib,"winmm.lib")			//����PlaySound����������ļ�
#pragma  comment(lib,"Msimg32.lib")		//���ʹ��TransparentBlt��������Ŀ��ļ�

#define WINDOW_WIDTH	800							
#define WINDOW_HEIGHT	600						
#define WINDOW_TITLE		L"Plane Game"	

enum DIR{LEFT,RIGHT,UP,DOWN};


HDC				g_hdc = NULL, g_mdc = NULL, g_bufdc = NULL;      //ȫ���豸���������ȫ���ڴ�DC���
DWORD		g_tPre = 0, g_tNow = 0;					//����l������������¼ʱ��,g_tPre��¼��һ�λ�ͼ��ʱ�䣬g_tNow��¼�˴�׼����ͼ��ʱ��
											 //һϵ��λͼ����Ķ���
HBITMAP		g_hBackGround;
HBITMAP		g_hBombLightingBitmap,g_hBullet0Bitmap,g_hBullet1Bitmap,
g_hBonusEnhancedBulletBitmap,g_hBonusBombSkillBitmap,
g_hMyPlaneBitmap, g_hEnermy0Bitmap, g_hEnermy1Bitmap,
g_hEnermy2Bitmap, g_hMyBulletBitmap, g_hEnermyBulletBitmap;  

Game g_game;//��Ϸ�࣬������Ϸ�еķɻ����ӵ��ȵĳ�ʼ�����жϺ�����

LRESULT CALLBACK	WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL						Game_Init(HWND hwnd);			//�ڴ˺����н�����Դ�ĳ�ʼ��
VOID							GameLoop(HWND hwnd);		//�ڴ˺����н��л�ͼ�������д�ͳ������еĺ��Ĵ���
BOOL						Game_ShutDown(HWND hwnd);	//�ڴ˺����н�����Դ������

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

	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//����������ʾʱ��λ�ã�ʹ�������Ͻ�λ�ڣ�250,80����
	ShowWindow(hwnd, nShowCmd);    
	UpdateWindow(hwnd);						

	if (!Game_Init(hwnd)){
		MessageBox(hwnd, L"��Դ��ʼ��ʧ��", L"��Ϣ����", 0); 
		return FALSE;
	}
	PlaySound(L"GameMedia\\ս��1-ɭ��.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); //ѭ�����ű������� 

	MSG msg = { 0 };				
	while (msg.message != WM_QUIT){
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);		//���������Ϣת��Ϊ�ַ���Ϣ
			DispatchMessage(&msg);			//�ַ�һ����Ϣ�����ڳ���
		}
		
		else{
		g_tNow = GetTickCount();   //��ȡ��ǰϵͳʱ��
		if (g_tNow - g_tPre >= 60)      //���˴�ѭ���������ϴλ�ͼʱ�����0.06��ʱ�ٽ����ػ����
			GameLoop(hwnd);
		}
	}

	UnregisterClass(L"MyPlane", wndClass.hInstance);  //����׼��������ע��������
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	PAINTSTRUCT paintStruct;

	switch (message){
	case WM_PAINT:
		g_hdc = BeginPaint(hwnd, &paintStruct);
			GameLoop(hwnd);
		EndPaint(hwnd, &paintStruct);
		ValidateRect(hwnd, NULL);		// ���¿ͻ�������ʾ
		break;
		
	case WM_KEYDOWN:					
		switch (wParam){
		case VK_ESCAPE:		//���¡�Esc����
			PostQuitMessage(0);
			break;
		case VK_RETURN:    //Enter �������۳�ʼ״̬������Ϸ��������enter������ʼ��Ϸ
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
			if(g_game.plane.pos.X<WINDOW_WIDTH-70)//70ֻ��һ���ܴ��ԵĹ��ƣ�������ֵ�����Ҳ�������ǹ���
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
		//case VK_CONTROL://Ctrl���������ӵ�
			//g_game.shoot();
			//break;
		case VK_SPACE://�ո����ը��
			g_game.bomb();
			break;
		default:
			break;
		}
		break;
	case WM_DESTROY:					//���Ǵ���������Ϣ
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

	//��������ϵ�Ĵ���
	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);  //����һ����hdc���ݵ��ڴ�DC
	g_bufdc = CreateCompatibleDC(g_hdc);//�ٴ���һ����hdc���ݵĻ���DC
	bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT); //��һ���ʹ��ڼ��ݵĿյ�λͼ����

	SelectObject(g_mdc, bmp);//����λͼ����ŵ�mdc��

							 //����һϵ����Ϸ��Դͼ��λͼ�����
	g_hBackGround = (HBITMAP)LoadImage(NULL, L"GameMedia\\bg.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);  //����λͼ
	g_hMyPlaneBitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\plane.bmp", IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE);  //�����ɫλͼ
	g_hEnermy0Bitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\enermy0.bmp", IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE);  //Ӣ�۽�ɫλͼ
	g_hEnermy1Bitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\enermy1.bmp", IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE);  //Ӣ�۽�ɫλͼ
	g_hEnermy2Bitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\enermy2.bmp", IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE);  //Ӣ�۽�ɫλͼ
	g_hMyBulletBitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\myBullet.bmp", IMAGE_BITMAP, 15, 15, LR_LOADFROMFILE);  //Ӣ�۽�ɫλͼ
	g_hBullet0Bitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\bullet0.bmp", IMAGE_BITMAP, 15, 15, LR_LOADFROMFILE);  //Ӣ�۽�ɫλͼ
	g_hBullet1Bitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\bullet1.bmp", IMAGE_BITMAP, 15, 15, LR_LOADFROMFILE);  //Ӣ�۽�ɫλͼ
	g_hBombLightingBitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\lightning.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);  //Ӣ�۽�ɫλͼ
	g_hEnermyBulletBitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\enermyBullet.bmp", IMAGE_BITMAP, 15, 15, LR_LOADFROMFILE);  //Ӣ�۽�ɫλͼ
	g_hBonusEnhancedBulletBitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\bonusEnhancedBullet.bmp", IMAGE_BITMAP, 25, 25, LR_LOADFROMFILE);  //�����ɫλͼ
	g_hBonusBombSkillBitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\bonusBombSkill.bmp", IMAGE_BITMAP, 25, 25, LR_LOADFROMFILE);  //�����ɫλͼ

																															//��������
	HFONT hFont;
	hFont = CreateFont(25, 0, 0, 0, 700, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("΢���ź�"));
	SelectObject(g_mdc, hFont);
	SetBkMode(g_mdc, TRANSPARENT);    //����������ʾ����͸��

	GameLoop(hwnd);  //����һ����ϷMain����
	return TRUE;
}

//-----------------------------------��GameLoop( )������--------------------------------------
//	��������ϷMain������������Ϸ���СС�����ĺ������������
//--------------------------------------------------------------------------------------------------
VOID GameLoop(HWND hwnd){
	wchar_t str[30];   //������ʱ�洢�ַ���������

	//����mdc�����ϱ���ͼ�����ñ���ѭ���ƶ�
	SelectObject(g_bufdc, g_hBackGround);
	static int nYOffset = 0;
	nYOffset = (nYOffset + 3) % WINDOW_HEIGHT;//3�����˱����ƶ����ٶ�
	for (int i = 0; i < 1; i++){
		for (int j = -1; j < 1; j++){
			BitBlt(g_mdc, i*WINDOW_WIDTH, j*WINDOW_HEIGHT + nYOffset, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);
		}
	}

	//��ʾ׼���׶�ʱ�Ľ���
	if (g_game.gameState == GAME_PREPARE) {
		SetTextColor(g_mdc, RGB(0, 255, 0));
		TextOut(g_mdc, 300, 250, L"Bullet: Ctrl", wcslen(L"Bullet: Ctrl"));
		TextOut(g_mdc, 300, 280, L"Bomb: Space", wcslen(L"Bomb: Space"));
		TextOut(g_mdc, 300, 310, L"Move: Direction Key", wcslen((L"Move: Direction Key")));
		//TextOut(g_mdc, 300, 340, L"Start: Enter", wcslen((L"Start: Enter")));
		TextOut(g_mdc, 300, 340, L"Exit: Esc", wcslen(L"Exit: Esc"));

		//��ʾ��˸�ذ�Enter������ʾ
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

		//���÷ɻ��޵�ģʽ�ĳ���ʱ�䣨���ƣ�
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

		if (g_game.planeSuperState == TRUE){ //�޵�ʱ����˸Ч��
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

		//�ڽ����Ϸ���ʾ����������ֵ��
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

		//��ʾ�л�����ʹ����ը��ʱ������ɻ��Եĵл���������������ɵл�
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
				//�����ҷ��ɻ��ӵ�ͼ
				TransparentBlt(g_mdc, g_game.bulletGroup[i].pos.X, g_game.bulletGroup[i].pos.Y, 15, 15, g_bufdc, 0, 0, 15, 15, RGB(0, 0, 0));
				//�ƶ��ӵ������ж��Ƿ����
				g_game.bulletGroup[i].pos.Y -= 30;
				if (g_game.bulletGroup[i].pos.Y < 0) {
					g_game.bulletGroup[i].flag = false;
				}
			}

		if (g_game.plane.skill & ENHANCED_BULLET) {
			for (int i = 0; i < BULLET_NUM; i++) {
				if (g_game.enhancedBullet0[i].flag) {
					//���ϼӳ��ӵ�ͼ
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
		
		//�趨�ӳ��ӵ�����ʱ�䣨���ƣ�
		if (g_game.plane.skill & ENHANCED_BULLET) {
			static	int g_EnhancedBulletCount = 0;
			if (g_EnhancedBulletCount == 30) {
				g_EnhancedBulletCount = 0;
				g_game.plane.skill &= ~ENHANCED_BULLET;
				g_game.closeEnhancedBullet();
			}
			g_EnhancedBulletCount++;
		}
		
		//�������bonus
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

		//����bonusͼ���ƶ�bonus���ж��Ƿ����
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
				//����ƶ�bonus����
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

		g_game.judgeEatBonus();//�ж��Ƿ�Ե�bonus
		g_game.judgeEnermyIsShoot();

		g_game.enermyMove();

		g_game.judgeCollide();

		g_game.judgePlaneIsShoot();

		//����ըʱ������ҷ��ɻ��������ӵ�
		SelectObject(g_bufdc, g_hEnermyBulletBitmap);
		for (int i = 0; i < ENERMY_NUM; i++) {
			for (int j = 0; j < BULLET_NUM; j++) {
				if (!g_game.bombIsUsed ||  g_game.enermyGroup[i]->bulletGroup[j].pos.Y >= g_game.plane.pos.Y + 200 || g_game.enermyGroup[i]->bulletGroup[j].pos.Y <= g_game.plane.pos.Y - 200) {
					if (g_game.enermyGroup[i]->bulletGroup[j].flag) {
						//���ϵл��ӵ�ͼ
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

		//��ʾʹ��ը���������ͼƬ��������һ��ʱ��
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

	//��ʾ��Ϸ������Ľ���
	else {
		SetTextColor(g_mdc, RGB(0, 255, 0));
		TextOut(g_mdc, 300, 250, L"Game   Over !", wcslen(L"Game   Over !"));
		memset(str, 0, sizeof(str));
		swprintf_s(str, L"Shoot down enermy planes: %d", g_game.enermyShot);
		TextOut(g_mdc, 300, 280, str, wcslen(str));
		memset(str, 0, sizeof(str));
		swprintf_s(str, L"Score: %d", g_game.score);
		TextOut(g_mdc, 300, 300, str, wcslen(str));

		//��ʾ��˸����ʾ
		static int enterCount = 0;
		if (enterCount >=8)
			TextOut(g_mdc, 300, 350, L"Press Enter to continue!", wcslen(L"Press Enter to continue!"));
		enterCount++;
		if (enterCount == 16)
			enterCount = 0;
		
	}

	//��mdc�е�ȫ����������hdc��
	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);
	g_tPre = GetTickCount();     //��¼�˴λ�ͼʱ��
}
BOOL Game_ShutDown(HWND hwnd)
{
	//�ͷ���Դ����
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