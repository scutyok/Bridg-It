#include "graphics.h"
#include <vector>
#include <iostream>
#include <windows.h>
#include <string>
#include <thread>

using namespace std;

vector< vector<int> > map;

#define font 8
#define direction 0
#define font_size sH / 200
#define hover_font_size font_size + 2

int sH = GetSystemMetrics(SM_CYSCREEN);
int sW = GetSystemMetrics(SM_CXSCREEN);

int clearbuffer = 0;

int fullscreen = 1;


void genMap(int n)
{
	map.resize(n+1);
	for (int i = 0; i < n; i++)
	{
		map[i].resize(n+1);
	}

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
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

int GUI(char text[], int diffx, int diffy)
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
	if (ismouseclick(WM_LBUTTONDOWN))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int charNr_to_int(char ch[])
{
	int nr = 0;
	for (int i = 0; i < strlen(ch); i++)
	{
		nr = nr * 10 + ch[i] - '0';
	}
	return nr;
}

void Fullscreen()
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
}

int main()
{
	initwindow(sW, sH, "", -3, -3);

	int activePage = 0;

	while (1)
	{
		Fullscreen();

		setactivepage(activePage);
		setvisualpage(1 - activePage);
		cleardevice();

		int PVP = 0;
		int PVAI = 0;

		PVP = GUI("Player V.S. Player", -400, 0);
		PVAI = GUI("Player V.S. AI", 400, 0);

		int numb[5];
		char ch[5];

		if (PVP==1)
		{
			setactivepage(0);
			setvisualpage(0);
			cleardevice();

			while(1)
			{
				int mapsize;
				char numbtxt[100] = { 'M', 'a', 'p', ' ', 'S', 'i', 'z', 'e', ':', ' '};
				outtextxy(sW/2-100, sH/2, numbtxt);
				do
				{
					int ok = 0;
					char aux;
					aux = getch();
					int ch_width;
					int text_width=textwidth(numbtxt);
					if (aux >= 48 && aux <= 57)
					{
						ok++;
						numb[0] = (int)aux;
						ch[0] = (int)aux;
						ch[1] = '\0';
						ch_width = textwidth(ch);
						outtextxy(sW/2 + text_width/3 + ch_width, sH/2, ch);
					}
					aux = getch();
					if (aux >= 48 && aux <= 57)
					{
						ok++;
						numb[1] = (int)aux;
						ch[1] = (int)aux;
						ch[2] = '\0';
						outtextxy(sW/2 + text_width/3 + ch_width*2, sH/2, ch+1);
					}
					if (ok == 2)
					{
						char key = getch();
						if (key < 48 || key > 57)
						{
							mapsize = charNr_to_int(ch);
							break;
						}
					}
					if (GetKeyState(VK_ESCAPE) & 0x8000)
					{
						break;
					}
				} while (1);

				genMap(mapsize);

				thread mapThread(genMap, mapsize);
				mapThread.join();

				setactivepage(0);
				setvisualpage(0);
				cleardevice();

				while (1)
				{
					for (int i = 0; i < mapsize; i++)
					{
						for (int j = 0; j < mapsize; j++)
						{
							int epsilon = sH / 10;
							if (map[i][j] == 1)
							{
								setcolor(RED);	
								circle(sW/2-mapsize/2*epsilon+j*epsilon, sH/2 - mapsize / 2 * epsilon +i*epsilon, 10);
								setfillstyle(SOLID_FILL, COLOR(202, 65, 65));
								floodfill(sW / 2 - mapsize / 2 * epsilon + j * epsilon, sH / 2 - mapsize / 2 * epsilon + i * epsilon, RED);
							}
							else if (map[i][j] == -1)
							{
								setcolor(BLUE);
								circle(sW/2-mapsize/2*epsilon+j*epsilon, sH/2 - mapsize / 2 * epsilon +i*epsilon, 10);
								setfillstyle(SOLID_FILL, COLOR(65, 65, 202));
								floodfill(sW / 2 - mapsize / 2 * epsilon + j * epsilon, sH / 2 - mapsize / 2 * epsilon + i * epsilon, BLUE);
							}
						}
					}

					Fullscreen();

					if (GetKeyState(VK_ESCAPE) & 0x8000)
					{
						break;
					}
				}
			}
		}

		activePage = 1 - activePage;

		if (GetKeyState(VK_ESCAPE) & 0x8000)
		{
			break;
		}

		delay(10);
	}
	return 0;
}
