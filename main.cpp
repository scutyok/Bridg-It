#include "graphics.h"
#include <vector>
#include <iostream>
#include <windows.h>
#include <string>
#include <thread>
#include <iomanip>
#include <cmath>
#include <queue>

using namespace std;

vector<vector<int>> map;
vector<vector<int>> p1road;
vector<vector<int>> p2road;

#define font 8
#define direction 0
#define font_size sH / 200
#define hover_font_size font_size + 2

int sH = GetSystemMetrics(SM_CYSCREEN);
int sW = GetSystemMetrics(SM_CXSCREEN);

int clearbuffer = 0;

int fullscreen = 1;

struct Point {
    int x, y;
};

vector<pair<Point, Point>> linii;
vector<pair<Point, Point>> pozlinii;

int di[5] = {0, 1, 0, -1};
int dj[5] = {1, 0, -1, 0};

void afisMap(int n, vector<vector <int>> a)
{
	cout << n << '\n';
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << setw(5) << a[i][j];
        }
        cout << '\n';
    }
}

void debugmaps(int n)
{
    cout << "mapa playerului 1: " << '\n';
    afisMap(n, p1road);
    cout << "mapa playerului 2: " <<'\n';
    afisMap(n, p2road);
}

void fill(int istart, int jstart, int n, int v, vector<vector <int>> a, int &win, int player)
{
    queue<pair<int, int>> Q;
    Q.push(make_pair(istart, jstart));
    a[istart][jstart] = v;
    while (!Q.empty())
    {
        int i = Q.front().first, j = Q.front().second;
        for (int k = 0; k < 4; k++)
        {
            int iv = i + di[k], jv = j + dj[k];
            if (iv >= 0 && iv < n && jv >= 0 && jv < n && a[iv][jv]==1 && a[iv][jv] != v)
            {
                a[iv][jv] = v;
                Q.push(make_pair(iv, jv));
				if (iv == n - 1 && player == 1)
				{
                    win = 1;
				}
				if (jv == n - 1 && player == -1)
				{
					win = 2;
				}
            }
        }
        Q.pop();
    }

    /*debugmaps(n);
	cout << "mapa din fill: " << '\n';
	afisMap(n, a);*/
}

void genMap(int n)
{
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
                settextstyle(font, direction, font_size);
                delay(100);
                break;
            case 0:
                closegraph();
                initwindow(sW, sH, "", -3, -3);
                fullscreen = 1;
                settextstyle(font, direction, font_size);
                delay(100);
                break;
        }
    }
}

bool exista(Point p1, Point p2)
{
    for (int i = 0; i < pozlinii.size(); i++)
    {
        pair<Point, Point>& line = pozlinii[i];
        if ((line.first.x == p1.x && line.first.y == p1.y && line.second.x == p2.x && line.second.y == p2.y) ||
            (line.first.x == p2.x && line.first.y == p2.y && line.second.x == p1.x && line.second.y == p1.y))
        {
            return true;
        }
    }
    return false;
}

void drawMap(int mapsize, int epsilon)
{
    POINT pnt;
    GetCursorPos(&pnt);
    ScreenToClient(GetForegroundWindow(), &pnt);

    for (int i = 0; i < mapsize; i++)
    {
        for (int j = 0; j < mapsize; j++)
        {
            int circleX = sW / 2 - mapsize / 2 * epsilon + j * epsilon;
            int circleY = sH / 2 - mapsize / 2 * epsilon + i * epsilon;
            int radius = 10;

            if (sqrt(pow(pnt.x - circleX, 2) + pow(pnt.y - circleY, 2)) <= radius)
            {
                radius = 15;
            }

            if (map[i][j] == 1)
            {
                setcolor(RED);
                circle(circleX, circleY, radius);
                setfillstyle(SOLID_FILL, COLOR(202, 65, 65));
                floodfill(circleX, circleY, RED);
            }
            else if (map[i][j] == -1)
            {
                setcolor(BLUE);
                circle(circleX, circleY, radius);
                setfillstyle(SOLID_FILL, COLOR(65, 65, 202));
                floodfill(circleX, circleY, BLUE);
            }
        }
    }
}

int intersectie(Point p1, Point p2, int epsilon, int mapsize)
{
    Point tp1;
    Point tp2;
	if (p1.x == p2.x)
	{
        if (p1.y != p2.y)
        {
            tp1 = {p1.x-1, (p1.y + p2.y) / 2 };
            tp2 = {p2.x+1, (p1.y + p2.y) / 2 };
        }
	}
	else if (p1.y == p2.y)
	{
		if (p1.x != p2.x)
		{
            tp1 = { (p1.x + p2.x) / 2, p1.y-1 };
            tp2 = { (p1.x + p2.x) / 2, p2.y+1 };
		}
	}
	if (exista(tp1,tp2))
    {
        return 1;
	}
    return 0;
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

        if (PVP == 1)
        {
            setactivepage(0);
            setvisualpage(0);
            cleardevice();

            while (1)
            {
                int mapsize;
                char numbtxt[100] = { 'M', 'a', 'p', ' ', 'S', 'i', 'z', 'e', ':', ' ' };
                outtextxy(sW / 2 - 100, sH / 2, numbtxt);
                do
                {
                    int ok = 0;
                    char aux;
                    aux = getch();
                    int ch_width;
                    int text_width = textwidth(numbtxt);
                    if (aux >= 48 && aux <= 57)
                    {
                        ok++;
                        numb[0] = (int)aux;
                        ch[0] = (int)aux;
                        ch[1] = '\0';
                        ch_width = textwidth(ch);
                        outtextxy(sW / 2 + text_width / 3 + ch_width * 2, sH / 2, ch);
                    }
                    if (ok == 1)
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

                if (GetKeyState(VK_ESCAPE) & 0x8000)
                {
                    break;
                }

                map.resize(mapsize);
                for (int i = 0; i < mapsize; i++)
                {
                    map[i].resize(mapsize);
                }

                genMap(mapsize);

                thread mapThread(genMap, mapsize);
                mapThread.join();

                setactivepage(0);
                setvisualpage(0);
                cleardevice();

                int player = 1;
                int generare = 0;
                Point firstClick = { -1, -1 };
                int page = 0;
                settextstyle(font, direction, font_size);
                int win = 0;

                linii.clear();
                pozlinii.clear();
				p1road.clear();
				p2road.clear();

                p1road.resize(mapsize);
                for (int i = 0; i < mapsize; i++)
                {
                    p1road[i].resize(mapsize);
                }

                p2road.resize(mapsize);
                for (int i = 0; i < mapsize; i++)
                {
                    p2road[i].resize(mapsize);
                }

                while (1)
                {
                    int epsilon = sH / 10;

                    drawMap(mapsize, epsilon);

                    int playercolor = -1;
                    for (int i = 0; i < linii.size(); i++)
                    {
                        const auto& linePair = linii[i];
                        if (playercolor == 1)
                        {
                            setcolor(COLOR(65, 65, 202));
                            playercolor = -playercolor;
                            
                        }
						else if (playercolor == -1)
                        {
                            setcolor(COLOR(202, 65, 65));
							playercolor = -playercolor;
                        }
                        line(linePair.first.x, linePair.first.y, linePair.second.x, linePair.second.y);
                    }

                    setactivepage(page);
                    setvisualpage(1 - page);
                    cleardevice();

                    POINT pnt;
                    GetCursorPos(&pnt);
                    ScreenToClient(GetForegroundWindow(), &pnt);

                    int linepage = 0;

                    if (ismouseclick(WM_LBUTTONDOWN))
                    {
                        int clickX = pnt.x;
                        int clickY = pnt.y;
                        clearmouseclick(WM_LBUTTONDOWN);

                        setactivepage(linepage);
                        setvisualpage(1 - linepage);
                        cleardevice();

                        for (int i = 0; i < mapsize; i++)
                        {
                            for (int j = 0; j < mapsize; j++)
                            {
                                int circleX = sW / 2 - mapsize / 2 * epsilon + j * epsilon;
                                int circleY = sH / 2 - mapsize / 2 * epsilon + i * epsilon;
                                if (sqrt(pow(clickX - circleX, 2) + pow(clickY - circleY, 2)) <= 10)
                                {
                                    if (firstClick.x == -1 && firstClick.y == -1)
                                    {
                                        firstClick = { j, i };
                                    }
                                    else
                                    {
                                        Point secondClick = { j, i };
                                        if ((abs(firstClick.x - secondClick.x) == 2 && firstClick.y == secondClick.y) ||
                                            (abs(firstClick.y - secondClick.y) == 2 && firstClick.x == secondClick.x)) 
                                        { 

                                            bool canDraw = true;

                                            if (canDraw && !exista(firstClick, secondClick) && map[firstClick.y][firstClick.x] == player && map[secondClick.y][secondClick.x] == player && !intersectie(firstClick, secondClick, epsilon, mapsize))
                                            {
                                                linii.push_back({ {sW / 2 - mapsize / 2 * epsilon + firstClick.x * epsilon, sH / 2 - mapsize / 2 * epsilon + firstClick.y * epsilon},
                                                                  {sW / 2 - mapsize / 2 * epsilon + secondClick.x * epsilon, sH / 2 - mapsize / 2 * epsilon + secondClick.y * epsilon} });
												pozlinii.push_back({ firstClick, secondClick });
                                                if (player == 1)
                                                {
													p1road[firstClick.y][firstClick.x] = 1;
													p1road[secondClick.y][secondClick.x] = 1;
													if (firstClick.y == secondClick.y && firstClick.x != secondClick.x)
													{

														p1road[firstClick.y][max(firstClick.x,secondClick.x)-1] = 1;
                                                    }
                                                    if (firstClick.y != secondClick.y && firstClick.x == secondClick.x)
                                                    {

                                                        p1road[max(firstClick.y, secondClick.y) - 1][firstClick.x] = 1;
                                                    }
                                                    for (int i = 0; i < mapsize; i++)
                                                    {
                                                        if (p1road[0][i] == 1)
                                                        {
                                                            fill(0, i, mapsize, 0, p1road, win, player);
                                                        }
                                                    }
												}
                                                if (player == -1)
                                                {
                                                    p2road[firstClick.y][firstClick.x] = 1;
                                                    p2road[secondClick.y][secondClick.x] = 1;
                                                    if (firstClick.y == secondClick.y && firstClick.x != secondClick.x)
                                                    {

                                                        p2road[firstClick.y][max(firstClick.x, secondClick.x) - 1] = 1;
                                                    }
                                                    if (firstClick.y != secondClick.y && firstClick.x == secondClick.x)
                                                    {

                                                        p2road[max(firstClick.y, secondClick.y) - 1][firstClick.x] = 1;
                                                    }
                                                    for (int i = 0; i < mapsize; i++)
                                                    {
                                                        if (p2road[i][0] == 1)
                                                        {
                                                            fill(i, 0, mapsize, 0, p2road, win, player);
                                                        }
                                                    }
                                                }
                                                switch (win)
                                                {
                                                    case 1:
                                                        setactivepage(0);
                                                        setvisualpage(0);
                                                        cleardevice();
                                                        outtextxy(sW / 2 - textwidth("Red wins!")/2, sH / 2, "Red wins!");
                                                        while (1)
                                                        {
                                                            if (GetKeyState(VK_ESCAPE) & 0x8000)
                                                            {
                                                                setcolor(COLOR(255, 255, 255));
                                                                break;
                                                            }
                                                        }
                                                        break;
                                                    case 2:
                                                        setactivepage(0);
                                                        setvisualpage(0);
                                                        cleardevice();
                                                        outtextxy(sW / 2 - textwidth("Blue wins!")/2, sH / 2, "Blue wins!");
                                                        while (1)
                                                        {
                                                            if (GetKeyState(VK_ESCAPE) & 0x8000)
                                                            {
                                                                setcolor(COLOR(255, 255, 255));
                                                                break;
                                                            }
                                                        }
                                                        break;
                                                }
                                                firstClick = { -1, -1 };
                                                player = -player;
                                            }
                                            else 
                                            {
                                                firstClick = { -1, -1 };
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        if (player == 1)
                        {
                            setcolor(COLOR(65, 65, 202));
                        }
                        else
                        {
                            setcolor(COLOR(202, 65, 65));
                        }
                        linepage = 1 - linepage;
                    }

                    if (player == -1)
                    {
                        outtextxy(sW / 8, sH / 2, "Turn: Blue");
                    }
                    if (player == 1)
                    {
                        outtextxy(sW / 8, sH / 2, "Turn: Red");
                    }

                    page = 1 - page;

                    Fullscreen();

                    if (GetKeyState(VK_ESCAPE) & 0x8000)
                    {
                        setcolor(COLOR(255, 255, 255));
                        break;
                    }
                }
            }
        }

        activePage = 1 - activePage;

        delay(10);
    }
    return 0;
}
