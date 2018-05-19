#include <Windows.h>
#include <iostream>
#include <random>
#include <math.h>
#include <stdio.h>
#include <WinUser.h>
#include "resource.h"

#pragma comment(lib,"winmm.lib")

using namespace std;

//RECT rtRect;
//
//GetClientRect(hWnd, rtRect); // 마우스 못나가게 할 영역 reRect로 설정
//
//
//ClipCursor(&rtRect); //윈도우 밖으로 못나게 함 (영역을 클립 시킨다)
//
//
//ClipCursor(NULL); // 사용후 해제
//

#define Height 1920
#define Weight 1080

using namespace std;
HPEN MyPen, OldPen;
#define MAX 9
HINSTANCE g_hinst;
LPCTSTR lpszClass = "Windows Programing KWB";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hinst = hInstance;
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);
	hWnd = CreateWindow(lpszClass, "window progra1-2", WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL, 0, -20, Height, Weight, NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}


double LengthCheck(double mx, double my, double x, double y)
{
	return sqrt((mx - x)*(mx - x) + (my - y)*(my - y));
}
BOOL InCircle(double mx, double my, double x, double y, double r)
{
	if (LengthCheck(mx, my, x, y) < r)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

float Check(double x, double y, double lx, double ly, double mx)
{
	return (double)(ly - y) / (double)(lx - x)*(double)(mx - lx) + (double)ly;
}

struct Myball {
	double x;
	double y;
	double rad = 20;
	int count = 0;
	BOOL ballCheck = FALSE;
};

struct Virus {
	double x;
	double y;
	int rad = 10;
	int lotate;
	double startX;
	double startY;
	double Maxlenth = 3982478400;
	double length;
	int hitCount = 0;
};

struct DivisionVirus {
	Virus virus[100];
	int number;
	double virusTime = 0;
};

struct Food {
	double x;
	double y;
	double startX;
	double startY;
	double endX;
	double endY;
	double xrate;
	double yrate;
	BOOL destination = FALSE;
};

struct Trap {
	double x;
	double y;
	int lotate = -1;
	double effectRad = 0;
	double rad = 20;
};

struct redZone {
	double x = - 1000;
	double y = - 1000;
	double rad = 100;
	int count = 0;
	BOOL RedZone = FALSE;
};
// 원이 분열하는건 원 갯수만큼 for문을 돌려서 분열한다@!
// 원이 튕기게 하기위해서 우선 벽들간 충돌을 확인해보고! autoSelectLocate 변수를 바꿔준다.

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static int Game = 0;
	static int plusX = 980;
	static int plusY = 540;
	static BOOL FixMode = FALSE;
	PAINTSTRUCT ps;
	HDC hdc;
	static int randDis; // 바이러스 분열할때 랜덤방향으로 생깁네당
	static double Zoom;
	static Myball ball;
	static Myball division[100];
	static DivisionVirus divisionVirus[100];
	static HBITMAP hbmOld, hbmMem, hbmMemOld , hbmMemOld2, myBitmap, oldBitmap;
	static RECT clientRECT, changeCameraMode;
	static HDC memDC, hmemdc;
	static HBITMAP hBitmap, hBitmap2;
	static HPEN hpen, oldpen;
	static HBRUSH hBrush, oldBrush;
	static BOOL Drag;
	static Food food[60];
	static Food foodout[1000];
	static Virus virus[1000];
	static Trap trap[50];
	static int trapCount = 3;
	static double positionX, positionY;
	static double divisionPointX, divisionPointY;
	static double startX, startY;
	static double oldX, oldY;
	static int virusCount = 1;
	static double length;
	static double xrate, yrate;
	static double virusXrate[100], virusYrate[100];
	static double divisionXrate[100], divisionYrate[100];
	static double trapXrate[100], trapYrate[100];
	static char test[100];
	static char foodText[100];
	static float Fixation;
	static double foodoutX, foodoutY;
	static int foodoutCount = 0;
	static int divisionCount = 0;
	static int divisionTime = 0;
	static int temp = 0;
	static char virusText[100];
	static char tim[100];
	static double tempRad;
	static int divisionVirusCount = 0;
	static int time = 0;
	static int inputX, inputY;
	static redZone redzone;


	switch (iMessage) {

	case WM_CREATE:
	reGame:
		Game = 0;
		time = 0;
		ball.rad = 20;
		GetClientRect(hWnd, &clientRECT);
		GetClientRect(hWnd, &changeCameraMode);
		PlaySound(MAKEINTRESOURCE(107), g_hinst, SND_RESOURCE | SND_ASYNC); // 148초짜리 브금임
		ball.x = 960;
		ball.y = 540;

		for (int i = 0; i < 60; ++i) {
			food[i].x = rand() % 1900 + 10;
			food[i].y = rand() % 1000 + 10;
		}
		virus[0].x = rand() % 1800 + 10;
		virus[0].y = rand() % 1000 + 10;
		//virus[0].lotate = rand() % 60; //이젠 안쓰는 코드 ㅎ
		for (int i = 0; i < 60; ++i) {
			virus[0].length = (sqrt((virus[0].x - food[i].x) * (virus[0].x - food[i].x) + (virus[0].y - food[i].y) * (virus[0].y - food[i].y)));
			if (virus[0].length < virus[0].Maxlenth) {
				virus[0].Maxlenth = virus[0].length;
				virus[0].lotate = i;
			}
		}
		virus[0].startX = virus[0].x;
		virus[0].startY = virus[0].y;
		virus[0].rad = 20;
		for (int i = 0; i < 3; ++i) {
			trap[i].x = rand() % 1800 + 10;
			trap[i].y = rand() % 1000 + 10;
		}

		break;

	case WM_TIMER:
		switch (wParam) {
		case 1:
			xrate = (abs(positionX - startX) / (sqrt((positionX - startX) * (positionX - startX) + (positionY - startY) * (positionY - startY)))) / (ball.rad / 10) * 6;
			yrate = (abs(positionY - startY) / (sqrt((positionX - startX) * (positionX - startX) + (positionY - startY) * (positionY - startY)))) / (ball.rad / 10) * 6;


			if (positionX >= ball.x && positionY >= ball.y)
			{
				ball.x += xrate;
				ball.y += yrate;
				KillTimer(hWnd, 3);
				SetTimer(hWnd, 3, 10, NULL);
			}
			else if (positionX >= ball.x && positionY < ball.y)
			{
				ball.x += xrate;
				ball.y -= yrate;
				KillTimer(hWnd, 3);
				SetTimer(hWnd, 3, 10, NULL);
			}
			else if (positionX < ball.x && positionY < ball.y)
			{
				ball.x -= xrate;
				ball.y -= yrate;
				KillTimer(hWnd, 3);
				SetTimer(hWnd, 3, 10, NULL);
			}
			else if (positionX < ball.x && positionY >= ball.y)
			{
				ball.x -= xrate;
				ball.y += yrate;
				KillTimer(hWnd, 3);
				SetTimer(hWnd, 3, 10, NULL);
			}
			

			for (int i = 0; i < 60; ++i) {
				if (InCircle(food[i].x, food[i].y, ball.x, ball.y, ball.rad) == TRUE) {
					food[i].x = rand() % 1900 + 10;
					food[i].y = rand() % 1000 + 10;
					if(ball.rad <= 100)
						ball.rad += 1;
					KillTimer(hWnd, 3);
					SetTimer(hWnd, 3, 10, NULL);
				}

			}
			for (int i = 0; i < foodoutCount; ++i) {
				if (InCircle(foodout[i].startX, foodout[i].startY, ball.x, ball.y, ball.rad) == TRUE && foodout[i].destination == TRUE) {
					if (ball.rad <= 100)
						ball.rad += 1;
					for (int j = i; j < foodoutCount; ++j) {
						foodout[j].startX = foodout[j + 1].startX;
						foodout[j].startY = foodout[j + 1].startY;
						foodout[j].endX = foodout[j + 1].endX;
						foodout[j].endY = foodout[j + 1].endY;
						foodout[j].xrate = foodout[j + 1].xrate;
						foodout[j].yrate = foodout[j + 1].yrate;
					}
					foodoutCount--;
					KillTimer(hWnd, 3);
					SetTimer(hWnd, 3, 10, NULL);
				}
			}

			for (int p = 0; p < trapCount; ++p) {
				if (trap[p].lotate == -1 || InCircle(trap[p].x, trap[p].y, food[trap[p].lotate].x, food[trap[p].lotate].y, trap[p].rad) == TRUE) {
					trap[p].lotate = rand() % 60;
				}
				else {
					trapXrate[p] = (abs(trap[p].x - food[trap[p].lotate].x) / (sqrt((trap[p].x - food[trap[p].lotate].x) * (trap[p].x - food[trap[p].lotate].x) + (trap[p].y - food[trap[p].lotate].y) * (trap[p].y - food[trap[p].lotate].y)))) / (trap[p].rad / 10) * 2;
					trapYrate[p] = (abs(trap[p].y - food[trap[p].lotate].y) / (sqrt((trap[p].x - food[trap[p].lotate].x) * (trap[p].x - food[trap[p].lotate].x) + (trap[p].y - food[trap[p].lotate].y) * (trap[p].y - food[trap[p].lotate].y)))) / (trap[p].rad / 10) * 2;

					if (food[trap[p].lotate].x >= trap[p].x && food[trap[p].lotate].y >= trap[p].y) {
						trap[p].x += trapXrate[p];
						trap[p].y += trapYrate[p];
					}
					else if (food[trap[p].lotate].x >= trap[p].x && food[trap[p].lotate].y < trap[p].y) {
						trap[p].x += trapXrate[p];
						trap[p].y -= trapYrate[p];
					}
					else if (food[trap[p].lotate].x < trap[p].x && food[trap[p].lotate].y < trap[p].y) {
						trap[p].x -= trapXrate[p];
						trap[p].y -= trapYrate[p];
					}
					else if (food[trap[p].lotate].x < trap[p].x && food[trap[p].lotate].y >= trap[p].y) {
						trap[p].x -= trapXrate[p];
						trap[p].y += trapYrate[p];
					}
					}
				}

			for (int p = 0; p < trapCount; ++p) {
				if (InCircle(trap[p].x, trap[p].y, ball.x, ball.y, ball.rad) == TRUE && ball.rad > trap[p].rad && ball.ballCheck == FALSE) {
					divisionTime = 0;
					ball.ballCheck = TRUE;
					tempRad = ball.rad;
					for (int l = p; l < trapCount; ++l) {
						trap[l].effectRad = trap[l + 1].effectRad;
						trap[l].lotate = trap[l + 1].lotate;
						trap[l].rad = trap[l + 1].rad;
						trap[l].x = trap[l + 1].x;
						trap[l].y = trap[l + 1].y;
					}
					for (int o = 0; o < 4; o++)
						ball.rad /= 1.414;
					divisionCount = 4;
					for (int k = 0; k < 4; ++k) {
						if (k == 0) {
							division[k].x = ball.x + ball.rad ;
							division[k].y = ball.y + ball.rad ;
							division[k].rad = ball.rad;
						}
						else if (k == 1) {
							division[k].x = ball.x + ball.rad ;
							division[k].y = ball.y - ball.rad ;
							division[k].rad = ball.rad;
						}
						else if (k == 2) {
							division[k].x = ball.x - ball.rad ;
							division[k].y = ball.y + ball.rad ;
							division[k].rad = ball.rad;
						}
						else if (k == 3) {
							division[k].x = ball.x - ball.rad ;
							division[k].y = ball.y - ball.rad ;
							division[k].rad = ball.rad;

						}
					}
					SetTimer(hWnd, 6, 1, NULL);
				}
			}

			if (InCircle(positionX, positionY, ball.x, ball.y, 10) == TRUE)
				//KillTimer(hWnd, 1);
				;
			
			
			InvalidateRect(hWnd, NULL, FALSE);
			break;

		case 2:
			virus[virusCount].x = rand() % 1800 + 10;
			virus[virusCount].y = rand() % 1800 + 10;
			virus[virusCount].lotate = rand() % 60;
			virus[virusCount].startX = virus[virusCount].x;
			virus[virusCount].startY = virus[virusCount].y;
			virusCount++;
			KillTimer(hWnd, 3);
			SetTimer(hWnd, 3, 10, NULL);

			break;

		case 3:
			for (int i = 0; i < virusCount; ++i) {

				for (int j = 0; j < 60; ++j) {
					virus[i].length = (sqrt((virus[i].x - food[j].x) * (virus[i].x - food[j].x) + (virus[i].y - food[j].y) * (virus[i].y - food[j].y)));
					if (virus[i].length < virus[i].Maxlenth) {
						virus[i].Maxlenth = virus[i].length;
						virus[i].lotate = j;
					}
				}






				virusXrate[i] = (abs(virus[i].x - food[virus[i].lotate].x) / (sqrt((virus[i].x - food[virus[i].lotate].x) * (virus[i].x - food[virus[i].lotate].x) + (virus[i].y - food[virus[i].lotate].y) * (virus[i].y - food[virus[i].lotate].y)))) / (virus[i].rad / 10) * 6;
				virusYrate[i] = (abs(virus[i].y - food[virus[i].lotate].y) / (sqrt((virus[i].x - food[virus[i].lotate].x) * (virus[i].x - food[virus[i].lotate].x) + (virus[i].y - food[virus[i].lotate].y) * (virus[i].y - food[virus[i].lotate].y)))) / (virus[i].rad / 10) * 6;
				if (food[virus[i].lotate].x >= virus[i].x && food[virus[i].lotate].y >= virus[i].y) {
					virus[i].x += virusXrate[i];
					virus[i].y += virusYrate[i];
				}
				else if (food[virus[i].lotate].x >= virus[i].x && food[virus[i].lotate].y < virus[i].y) {
					virus[i].x += virusXrate[i];
					virus[i].y -= virusYrate[i];
				}
				else if (food[virus[i].lotate].x < virus[i].x && food[virus[i].lotate].y < virus[i].y) {
					virus[i].x -= virusXrate[i];
					virus[i].y -= virusYrate[i];
				}
				else if (food[virus[i].lotate].x < virus[i].x && food[virus[i].lotate].y >= virus[i].y) {
					virus[i].x -= virusXrate[i];
					virus[i].y += virusYrate[i];
				}
				/*	for (int j = 0; j < 60; ++j) {
						if (InCircle(food[j].x, food[j].y, virus[i].x, virus[i].y, virus[i].rad) == TRUE)
						{
							virus[i].rad += 3;
							food[j].x = rand() % 1900 + 10;
							food[j].y = rand() % 1000 + 10;
						}
					}*/

				if (InCircle(food[virus[i].lotate].x, food[virus[i].lotate].y, virus[i].x, virus[i].y, virus[i].rad) == TRUE)
				{
					virus[i].Maxlenth = 3982478400;
					if(virus[i].rad<= 130)
						virus[i].rad += 1;
					else {
						virus[i].rad /= 2;
						virus[virusCount].x = virus[i].x + virus[i].rad;
						virus[virusCount].y = virus[i].y + virus[i].rad;
						virus[virusCount].rad = virus[i].rad;
						virus[virusCount].startX = virus[virusCount].x;
						virus[virusCount].startY = virus[virusCount].y;

						virusCount++;
					}
					food[virus[i].lotate].x = rand() % 1900 + 10;
					food[virus[i].lotate].y = rand() % 1000 + 10;
					//	
					for (int p = 0; p < 60; ++p) {
						virus[i].length = (sqrt((virus[i].x - food[p].x) * (virus[i].x - food[p].x) + (virus[i].y - food[p].y) * (virus[i].y - food[p].y)));
						if (virus[i].length < virus[i].Maxlenth) {
							virus[i].Maxlenth = virus[i].length;
							virus[i].lotate = p;
						}
					}
					//virus[i].startX = virus[i].x;
					//virus[i].startY = virus[i].y;
				}

				if (InCircle(ball.x, ball.y, virus[i].x, virus[i].y, ball.rad) == TRUE) {
					if (virus[i].rad > ball.rad) {
						Game = 2;
					}
					else {
						for (int j = i; j < virusCount - 1; ++j) {
							virus[j].lotate = virus[j + 1].lotate; // 가장 가까운걸로 가는건 FOR문 돌려서 거리로 합시다.
							virus[j].rad = virus[j + 1].rad;
							virus[j].startX = virus[j + 1].startX;
							virus[j].startY = virus[j + 1].startY;
							virus[j].x = virus[j + 1].x;
							virus[j].y = virus[j + 1].y;
							ball.rad += virus[j].rad / 2;
						}
						virusCount -= 1;
					}
				}

				for (int k = 0; k < foodoutCount; ++k) {
					if (InCircle(virus[i].x, virus[i].y, foodout[k].startX, foodout[k].startY, virus[i].rad) == TRUE && foodout[k].destination == TRUE) {
						if (virus[i].rad <= 130)
							virus[i].rad += 1;
						else {
							virus[i].rad /= 2;
							virus[virusCount].x = virus[i].x + virus[i].rad;
							virus[virusCount].y = virus[i].y + virus[i].rad;
							virus[virusCount].rad = virus[i].rad;
							virus[virusCount].startX = virus[virusCount].x;
							virus[virusCount].startY = virus[virusCount].y;

							virusCount++;
						}

						for (int n = k; n < foodoutCount - 1; ++n) {
							foodout[n].startX = foodout[n + 1].startX;
							foodout[n].startY = foodout[n + 1].startY;
							foodout[n].endX = foodout[n + 1].endX;
							foodout[n].endY = foodout[n + 1].endY;
							foodout[n].xrate = foodout[n + 1].xrate;
							foodout[n].yrate = foodout[n + 1].yrate;
						}
						foodoutCount--;
					}
				}

				for (int k = 0; k < divisionCount; ++k) {
					if (InCircle(division[k].x, division[k].y, virus[i].x, virus[i].y, virus[i].rad) == TRUE) {
						for (int o = k; o < divisionCount; ++o) {
							division[o].x = division[o + 1].x;
							division[o].y = division[o + 1].y;
							division[o].rad = division[o + 1].rad;
							division[o].count = division[o + 1].count;
	
						}
						tempRad /= 1.414;
					}
				}

				for (int p = 0; p < trapCount; ++p) {
					if (InCircle(trap[p].x, trap[p].y, virus[i].x, virus[i].y, virus[i].rad) == TRUE) {
						if (trap[p].rad > virus[i].rad) { // 본인보다 큰 트랩을 만나면 그냥 죽어욧!
							for (int j = i; j < virusCount - 1; ++j) {
								virus[j].lotate = virus[j + 1].lotate;
								virus[j].rad = virus[j + 1].rad;
								virus[j].startX = virus[j + 1].startX;
								virus[j].startY = virus[j + 1].startY;
								virus[j].x = virus[j + 1].x;
								virus[j].y = virus[j + 1].y;

							}
							virusCount -= 1;
						}
						else {
							virus[i].rad /= 2;
							virus[virusCount].x = virus[i].x + virus[i].rad;
							virus[virusCount].y = virus[i].y + virus[i].rad;
							virus[virusCount].rad = virus[i].rad;
							virus[virusCount].startX = virus[virusCount].x;
							virus[virusCount].startY = virus[virusCount].y;

							virusCount++;
							for (int k = p; k < trapCount; ++k) {
								trap[k].x = trap[k + 1].x;
								trap[k].y = trap[k + 1].y;
								trap[k].rad = trap[k + 1].rad;
								trap[k].effectRad = trap[k + 1].effectRad;
								trap[k].lotate = trap[k + 1].lotate;
								trapXrate[k] = trapXrate[k + 1];
								trapYrate[k] = trapYrate[k + 1];
							}
							trapCount--;
						}
					}
				}

				
			}
			break;

		case 4: // 함정이 생성이펙트!
			for (int i = 0; i < trapCount; ++i) {
				if (trap[i].rad != trap[i].effectRad)
					trap[i].effectRad += 1;
			}
			break;

		case 5: // 뱉은 먹이 이동하는곳
			for (int i = 0; i < foodoutCount; ++i) {
				foodout[i].xrate = ((abs(foodout[i].endX - foodout[i].startX) / (sqrt((foodout[i].endX - foodout[i].startX) * (foodout[i].endX - foodout[i].startX) + (foodout[i].endY - foodout[i].startY) * (foodout[i].endY - foodout[i].startY))))) * 10;
				foodout[i].yrate = ((abs(foodout[i].endY - foodout[i].startY) / (sqrt((foodout[i].endX - foodout[i].startX) * (foodout[i].endX - foodout[i].startX) + (foodout[i].endY - foodout[i].startY) * (foodout[i].endY - foodout[i].startY))))) * 10;
				if (InCircle(foodout[i].startX, foodout[i].startY, foodout[i].endX, foodout[i].endY, 5) != TRUE) {
					if (foodout[i].endX >= foodout[i].startX && foodout[i].endY >= foodout[i].startY)
					{
						foodout[i].startX += foodout[i].xrate;
						foodout[i].startY += foodout[i].yrate;
					}
					else if (foodout[i].endX >= foodout[i].startX && foodout[i].endY < foodout[i].startY)
					{
						foodout[i].startX += foodout[i].xrate;
						foodout[i].startY -= foodout[i].yrate;
					}
					else if (foodout[i].endX < foodout[i].startX && foodout[i].endY < foodout[i].startY)
					{
						foodout[i].startX -= foodout[i].xrate;
						foodout[i].startY -= foodout[i].yrate;
					}
					else if (foodout[i].endX < foodout[i].startX && foodout[i].endY >= foodout[i].startY)
					{
						foodout[i].startX -= foodout[i].xrate;
						foodout[i].startY += foodout[i].yrate;
					}
				}
				else
					foodout[i].destination = TRUE;
			}
			break;

		case 6: // 분열된 먹이들이 먹어요! 뭐를? 먹이를 ㅎ
			if (divisionTime < 100) {
				for (int i = 0; i < divisionCount; ++i) {
					divisionXrate[i] = (abs(positionX - division[i].x) / (sqrt((positionX - division[i].x) * (positionX - division[i].x) + (positionY - division[i].y) * (positionY - division[i].y)))) / (division[i].rad / 10) * 6;
					divisionYrate[i] = (abs(positionY - division[i].y) / (sqrt((positionY - division[i].y) * (positionY - division[i].y) + (positionY - division[i].y) * (positionY - division[i].y)))) / (division[i].rad / 10) * 6;
					if (positionX >= ball.x && positionY >= ball.y)
					{
						division[i].x += divisionXrate[i];
						division[i].y += divisionYrate[i];

					}
					else if (positionX >= ball.x && positionY < ball.y)
					{
						division[i].x += divisionXrate[i];
						division[i].y -= divisionYrate[i];

					}
					else if (positionX < ball.x && positionY < ball.y)
					{
						division[i].x -= divisionXrate[i];
						division[i].y -= divisionYrate[i];

					}
					else if (positionX < ball.x && positionY >= ball.y)
					{
						division[i].x -= divisionXrate[i];
						division[i].y += divisionYrate[i];

					}

					for (int j = 0; j < 60; ++j) {
						if (InCircle(division[i].x, division[i].y, food[j].x, food[j].y, division[i].rad)) {
							division[i].rad += 1;
							if(tempRad <= 100)
								tempRad += 1;
							food[j].x = rand() % 1900 + 10;
							food[j].y = rand() % 1000 + 10;
							KillTimer(hWnd, 3);
							SetTimer(hWnd, 3, 10, NULL);
						}
					}
				}
				divisionTime++;
			}
			else {
				for (int i = 0; i < divisionCount; ++i) {
					division[i].rad = 0;
					division[i].x = 0;
					division[i].y = 0;
				}
				divisionCount = 0;
				ball.rad = tempRad;
				ball.ballCheck = FALSE;
				KillTimer(hWnd, 6);
			}
			break;

		case 7:
			trap[trapCount].x = rand() % 1900 + 10;
			trap[trapCount].y = rand() % 1000 + 10;
			trap[trapCount].effectRad = 0;
			trapCount++;
			KillTimer(hWnd, 4);
			SetTimer(hWnd, 4, 1, NULL);
		break;

		case 8:
			time += 1;

			redzone.rad += 50;
			redzone.count += 1;
			if(redzone.count > 10)
				redzone.RedZone = FALSE;

			if(time % 1300 == 0 && time != 0)
				PlaySound(MAKEINTRESOURCE(107), g_hinst, SND_RESOURCE | SND_ASYNC); // 약 130초짜리 브금임
			break;

		case 9:
			redzone.x = rand() % 1900 + 10;
			redzone.y = rand() % 1000 + 10;
			redzone.rad = 100;
			redzone.count = 0;
			redzone.RedZone = TRUE;
			if (InCircle(redzone.x, redzone.y, ball.x, ball.y, redzone.rad + 100) == TRUE) {
				Game = 2;
			}
			
			for (int i = 0; i < virusCount; ++i) {
				if (InCircle(redzone.x, redzone.y, virus[i].x, virus[i].y, redzone.rad  + 100) == TRUE && redzone.RedZone == TRUE) {
					for (int j = i; j < virusCount - 1; ++j) {
						virus[j].lotate = virus[j + 1].lotate;
						virus[j].rad = virus[j + 1].rad;
						virus[j].startX = virus[j + 1].startX;
						virus[j].startY = virus[j + 1].startY;
						virus[j].x = virus[j + 1].x;
						virus[j].y = virus[j + 1].y;
					}
					virusCount--;
				}
			}
			break;
		}
		break;

	case WM_CHAR:
		divisionPointX = LOWORD(lParam);
		divisionPointY = HIWORD(lParam);

		if (wParam == 'd' || wParam == 'D') { // 주인공 원이 분열합니다~~~~
			if (divisionCount == 0) {
				if (ball.rad / 2 > 5) {
					tempRad = ball.rad;
					division[divisionCount].x = ball.x + ball.rad;
					division[divisionCount].y = ball.y + ball.rad;
					division[divisionCount].rad = ball.rad / 1.414;
					divisionCount++;
					ball.rad /= 1.414;
					divisionTime = 0;
					KillTimer(hWnd, 6);
					SetTimer(hWnd, 6, 1, NULL);
				}
			}//0 , 1
			else {
				if (ball.rad / 2 > 5) {
					for (int i = 0; i < divisionCount; ++i) { // 2
						division[divisionCount + i].x = ball.x + ball.rad;
						division[divisionCount + i].y = ball.y + ball.rad;
						division[divisionCount + i].rad = division[i].rad / 1.414;
						division[i].rad /= 1.414;
						temp++;
					}
					ball.rad /= 1.414;
					divisionTime = 0;
					divisionCount += temp;
					temp = 0;
					KillTimer(hWnd, 6);
					SetTimer(hWnd, 6, 1, NULL);
				}
			}
		}
		else if (wParam == 'r' || wParam == 'R') {
			if (Game == 2)
				goto reGame;
		}

		break;
	case WM_RBUTTONDOWN:
		foodoutX = LOWORD(lParam);
		foodoutY = HIWORD(lParam);
		if (ball.rad > 20) {
			foodout[foodoutCount].startX = ball.x;
			foodout[foodoutCount].startY = ball.y;
			foodout[foodoutCount].endX = foodoutX;
			foodout[foodoutCount].endY = foodoutY;
			foodout[foodoutCount].destination = FALSE;
			ball.rad -= 1;

			foodoutCount++;
			KillTimer(hWnd, 5);
			SetTimer(hWnd, 5, 1, NULL);
		}
		break;

	case WM_MOUSEWHEEL:
	{
		if (Game == 1) {
			int Check = GET_WHEEL_DELTA_WPARAM(wParam);

			if (Check > 0)
			{
				if (Zoom < 1)
				{
					Zoom++;
				}
				FixMode = TRUE;
			}
			else
			{
				Zoom--;
				if (Zoom <= 0)
				{
					Zoom = 0;
					FixMode = FALSE;
				}
			}
		}
		//InvalidateRect(hWnd, NULL, FALSE);

	}
	case WM_LBUTTONDOWN:
		if (Game == 0) {
			Game = 1;
			SetTimer(hWnd, 1, 1, NULL);
			SetTimer(hWnd, 4, 1, NULL);
			SetTimer(hWnd, 7, 14000, NULL);
			SetTimer(hWnd, 2, 8000, NULL);
			SetTimer(hWnd, 3, 1, NULL);
			SetTimer(hWnd, 8, 100, NULL);
			SetTimer(hWnd, 9, 10000, NULL);
		}
		if (Game == 3) {
			inputX = LOWORD(lParam);
			inputY = HIWORD(lParam);

			if (inputX >= 67 && inputX <= 535 && inputY <= 284 && inputY >= 87) {
				GetClientRect(hWnd, &clientRECT);
				GetClientRect(hWnd, &changeCameraMode);
				ball.x = 960;
				ball.y = 540;

				for (int i = 0; i < 60; ++i) {
					food[i].x = rand() % 1900 + 10;
					food[i].y = rand() % 1000 + 10;
				}
				virus[0].x = rand() % 1800 + 10;
				virus[0].y = rand() % 1000 + 10;
				//virus[0].lotate = rand() % 60; //이젠 안쓰는 코드 ㅎ
				for (int i = 0; i < 60; ++i) {
					virus[0].length = (sqrt((virus[0].x - food[i].x) * (virus[0].x - food[i].x) + (virus[0].y - food[i].y) * (virus[0].y - food[i].y)));
					if (virus[0].length < virus[0].Maxlenth) {
						virus[0].Maxlenth = virus[0].length;
						virus[0].lotate = i;
					}
				}
				virus[0].startX = virus[0].x;
				virus[0].startY = virus[0].y;

				for (int i = 0; i < 3; ++i) {
					trap[i].x = rand() % 1800 + 10;
					trap[i].y = rand() % 1000 + 10;
				}
				Game = 1;
				SetTimer(hWnd, 1, 1, NULL);
				SetTimer(hWnd, 4, 1, NULL);
				SetTimer(hWnd, 7, 14000, NULL);
				SetTimer(hWnd, 2, 8000, NULL);
				SetTimer(hWnd, 3, 1, NULL);
				SetTimer(hWnd, 8, 100, NULL);
				SetTimer(hWnd, 9, 10000, NULL);
			}
			else if (inputX >= 67 && inputX <= 537 && inputY <= 498 && inputY >= 327) {
				Game = 1;
				SetTimer(hWnd, 1, 1, NULL);
				SetTimer(hWnd, 4, 1, NULL);
				SetTimer(hWnd, 7, 14000, NULL);
				SetTimer(hWnd, 2, 8000, NULL);
				SetTimer(hWnd, 3, 1, NULL);
				SetTimer(hWnd, 8, 100, NULL);
				SetTimer(hWnd, 9, 10000, NULL);
			}
			else if (inputX >= 67 && inputX <= 539 && inputY <= 706 && inputY >= 540) {
				PostQuitMessage(0);
			}
		}
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			if (Game == 1) {
				KillTimer(hWnd, 1);
				KillTimer(hWnd, 4);
				KillTimer(hWnd, 7);
				KillTimer(hWnd, 2);
				KillTimer(hWnd, 3);
				KillTimer(hWnd, 8);
				KillTimer(hWnd, 9);
				Game = 3;
			}
			else if (Game == 3) {
				Game = 1;
				SetTimer(hWnd, 1, 1, NULL);
				SetTimer(hWnd, 4, 1, NULL);
				SetTimer(hWnd, 7, 14000, NULL);
				SetTimer(hWnd, 2, 8000, NULL);
				SetTimer(hWnd, 3, 1, NULL);
				SetTimer(hWnd, 8, 100, NULL);
				SetTimer(hWnd, 9, 10000, NULL);
			}
		}
		InvalidateRect(hWnd, NULL, FALSE);
		
		break;

	case WM_MOUSEMOVE:    // 흰 바탕   
		positionX = LOWORD(lParam);
		positionY = HIWORD(lParam);

		startX = ball.x;
		startY = ball.y;
		break;

	case WM_LBUTTONUP:
		//KillTimer(hWnd, 1);
		//positionX = LOWORD(lParam);
		//positionY = HIWORD(lParam);
		//Drag = FALSE;
		//
		//startX = ball.x;
		//startY = ball.y;
		//SetTimer(hWnd, 1, 1, NULL);
		break;




	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		memDC = CreateCompatibleDC(hdc);
		hmemdc = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, 1920 + 1950, 1080 + 1100);
		hbmMemOld = (HBITMAP)SelectObject(memDC, hBitmap);
		SelectObject(memDC, (HBITMAP)hBitmap);
		if (Game == 0) {
			myBitmap = LoadBitmap(g_hinst, MAKEINTRESOURCE(101));
			oldBitmap = (HBITMAP)SelectObject(memDC, myBitmap);
			StretchBlt(hdc, 0, 0, 1920, 1080, memDC, 0, 0, 720, 450, SRCCOPY);
		}
		else if (Game == 1 || Game == 2) {
			
			if (FixMode == TRUE) {
				plusX = 980;
				plusY = 540;
			}
			else {
				plusX = 0;
				plusY = 0;
			}


			PatBlt(hdc, 0, 0, 1920 + 1920, 1080 + 1080, SRCCOPY);



			Rectangle(memDC, 0 + plusX, 0 + plusY, 1920 + plusX, 1080 + plusY);

			for (int i = 0; i < 16; i++) {
				MoveToEx(memDC, 120 * (i + 1) + plusX, 0 + plusY, NULL);
				LineTo(memDC, 120 * (i + 1) + plusX, 1080 + plusY);
			}
			// 960, 540
			for (int i = 0; i < 9; i++) {
				MoveToEx(memDC, 0 + plusX, 120 * (i + 1) + plusY, NULL);
				LineTo(memDC, 1920 + plusX, 120 * (i + 1) + plusY);
			}



			if (Game == 1) {
				hBrush = CreateSolidBrush(RGB(255, 255, 100));
				oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
				Ellipse(memDC, ball.x - ball.rad + plusX, ball.y - ball.rad + plusY, ball.x + ball.rad + plusX, ball.y + ball.rad + plusY);
			}

		
		
			for (int i = 0; i < divisionCount; ++i) {
				Ellipse(memDC, division[i].x - division[i].rad + plusX, division[i].y - division[i].rad + plusY, division[i].x + division[i].rad + plusX, division[i].y + division[i].rad + plusY);
			}
			DeleteObject(hBrush);

			hpen = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
			oldpen = (HPEN)SelectObject(memDC, hpen);
			hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
			oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
			Ellipse(memDC, redzone.x - redzone.rad, redzone.y - redzone.rad, redzone.x + redzone.rad, redzone.y + redzone.rad);

			hpen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
			oldpen = (HPEN)SelectObject(memDC, hpen);

			for (int i = 0; i < virusCount; ++i) {
				if (virus[i].hitCount == 0) {
					hBrush = CreateSolidBrush(RGB(255, 0, 0));
					oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
					Ellipse(memDC, virus[i].x - virus[i].rad + plusX, virus[i].y - virus[i].rad + plusY, virus[i].x + virus[i].rad + plusX, virus[i].y + virus[i].rad + plusY);
					DeleteObject(hBrush);
				}
			}

			for (int i = 0; i < divisionVirusCount; ++i) {
				for (int j = 0; j < 5; ++j) {
					hBrush = CreateSolidBrush(RGB(255, 0, 0));
					oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
					Ellipse(memDC, divisionVirus[i].virus[j].x - divisionVirus[i].virus[j].rad + plusX, divisionVirus[i].virus[j].y - divisionVirus[i].virus[j].rad + plusY, divisionVirus[i].virus[j].x + divisionVirus[i].virus[j].rad + plusX, divisionVirus[i].virus[j].y + divisionVirus[i].virus[j].rad + plusY);
					DeleteObject(hBrush);
				}
			}
			for (int i = 0; i < foodoutCount; ++i) {
				hBrush = CreateSolidBrush(RGB(0, 0, 255));
				oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
				Ellipse(memDC, foodout[i].startX - 5 + plusX, foodout[i].startY - 5 + plusY, foodout[i].startX + 5 + plusX, foodout[i].startY + 5 + plusY);
				DeleteObject(hBrush);
			}

			for (int i = 0; i < 60; ++i) {
				hBrush = CreateSolidBrush(RGB(0, 0, 255));
				oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
				Ellipse(memDC, food[i].x - 5 + plusX, food[i].y - 5 + plusY, food[i].x + 5 + plusX, food[i].y + 5 + plusY);
				DeleteObject(hBrush);
			}

			for (int i = 0; i < trapCount; ++i) {
				hBrush = CreateSolidBrush(RGB(0, 0, 0));
				oldBrush = (HBRUSH)SelectObject(memDC, hBrush);
				Ellipse(memDC, trap[i].x - trap[i].effectRad + plusX, trap[i].y - trap[i].effectRad + plusY, trap[i].x + trap[i].effectRad + plusX, trap[i].y + trap[i].effectRad + plusY);
				DeleteObject(hBrush);
			}
			if (Game == 2) { //게임 오버시에 -> ㅎㅎ
		//	myBitmap = LoadBitmap(g_hinst, MAKEINTRESOURCE(102));
		//	oldBitmap = (HBITMAP)SelectObject(memDC, myBitmap);
				KillTimer(hWnd, 8);
				FixMode = FALSE;
				wsprintf(tim, "총 진행시간 : %d.%d sec", (int)time / 10, (int)time % 10);
				SetBkColor(memDC, RGB(255, 255, 100));
				TextOut(memDC, 900, 540 , tim, strlen(tim));
				wsprintf(test, "다시 시작 하시려면 R키를 누르세요");
				SetBkColor(memDC, RGB(255, 255, 100));
				TextOut(memDC, 860, 560, test, strlen(test));
				//StretchBlt(hdc, 0, 0, 1920, 1080, memDC, 0, 0, 641, 336, SRCCOPY);
			}
			else {
				wsprintf(test, "현재 원의 반지름 사이즈 : %d", (int)ball.rad);
				SetBkColor(memDC, RGB(200, 100, 100));
				TextOut(memDC, 10 + plusX, 10 + plusY, test, strlen(test));
				wsprintf(tim, "진행 시간 : %d.%d sec", (int)time / 10, (int)time % 10);
				SetBkColor(memDC, RGB(200, 100, 100));
				TextOut(memDC, 10 + plusX, 25 + plusY, tim, strlen(tim));
			}

			

			Fixation = (clientRECT.right + 1 * 10) / clientRECT.right;
			if (FixMode == FALSE) {
				StretchBlt(hdc, 0, 0, 1920, 1080, memDC, 0, 0, 1920, 1080, SRCCOPY);
			}
			else if (FixMode == TRUE) {
				//	BitBlt(hdc, 0 , 0 , clientRECT.right, clientRECT.bottom, memDC, 0,0, WHITENESS);
				StretchBlt(hdc, 0, 0, clientRECT.right, clientRECT.bottom, memDC, ball.x - 320 + plusX, ball.y - 180 + plusY, 640, 360, SRCCOPY);
				//StretchBlt(hdc, 0, 0, clientRECT.right, clientRECT.bottom, memDC, 0, 0, 1920, 1080, SRCCOPY);
			}
		}
		

		else if (Game == 3) {
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 4);
			KillTimer(hWnd, 7);
			KillTimer(hWnd, 2);
			KillTimer(hWnd, 3);
			KillTimer(hWnd, 8);
			KillTimer(hWnd, 9);
			myBitmap = LoadBitmap(g_hinst, MAKEINTRESOURCE(103));
			oldBitmap = (HBITMAP)SelectObject(memDC, myBitmap);
			
			StretchBlt(hdc, 0, 0, 1920, 1080, memDC, 0, 0, 1280, 450, SRCCOPY);;
		}
		DeleteObject(hpen);
		DeleteObject(oldpen);
		DeleteDC(memDC);
		DeleteObject(hbmMemOld);
		DeleteDC(hdc);
		DeleteObject(hBitmap);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}