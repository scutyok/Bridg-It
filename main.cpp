#include "graphics.h"
#include <vector>
#include <iostream>
#include <windows.h>

using namespace std;

vector< vector<int> > map;

#define font 8
#define direction 0
#define font_size sH / 200
#define hover_font_size font_size + 2

int sH = GetSystemMetrics(SM_CYSCREEN);
int sW = GetSystemMetrics(SM_CXSCREEN);

int clearbuffer = 0;

void genMap(int sH, int n)
{
	map.resize(sH);
	for (int i = 0; i < sH; i++)
	{
		map[i].resize(sH);
	}

	for (int i = sH / 2 - n / 2; i < sH / 2 + n / 2; i++)
	{
		for (int j = sH / 2 - n / 2; j < sH / 2 + n / 2; j++)
		{
			if (i % 2 == 0 && j % 2 != 0)
			{
				map[i][j] = 1;
			}
			else if (i % 2 != 0 && j % 2 == 0)
			{
				map[i][j] = -1;
			}
			else
			{
				map[i][j] = 0;
			}
		}
	}
}

void GUI(char text[], int diffx, int diffy)
{
	int text_width = textwidth(text);
	int text_height = textheight(text);
	int tx = (sW / 2) + diffx;
	int ty = (sH / 2) + diffy;
	POINT pnt;
	GetCursorPos(&pnt);
	if (pnt.x >= tx - text_width / 2 && pnt.x <= tx + text_width / 2 && pnt.y >= ty - text_height / 2 && pnt.y <= ty + text_height / 2)
	{
		settextstyle(font, direction, hover_font_size);
	}
	else
	{
		settextstyle(font, direction, font_size);
	}
	outtextxy(tx - text_width / 2, ty - text_height / 2, text);
}

int main()
{
	initwindow(sW, sH, "", -3, -3);
	int fullscreen = 1;
	genMap(sH, 9);

	int activePage = 0;

	while (1)
	{
		if (GetKeyState(VK_RETURN) & 0x8000)
		{
			switch (fullscreen)
			{
				case 1:
					closegraph();
					initwindow(sW, sH, "Bridg-It");
					fullscreen = 0;
					delay(100);
					break;
				case 0:
					closegraph();
					initwindow(sW, sH, "", -3, -3);
					fullscreen = 1;
					delay(100);
					break;
			}
		}

		setactivepage(activePage);
		setvisualpage(1 - activePage);
		cleardevice();

		GUI("Player V.S. Player", -400, 0);
		GUI("Player V.S. AI", 400, 0);

		activePage = 1 - activePage;

		if (GetKeyState(VK_ESCAPE) & 0x8000)
		{
			break;
		}

		delay(10);
	}
	return 0;
}
