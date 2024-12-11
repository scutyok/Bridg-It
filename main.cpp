#include "graphics.h"
#include <vector>
#include <iostream>
#include <windows.h>
#include <string>
#include <thread>
#include <iomanip>
#include <cmath>
#include <queue>
#include <set>
#include <time.h>

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
int BotDifficulty = 1;
int fullscreen = 1;

struct Point {
    int x, y;
};

struct muchie {
    int x, y, cost;
}g[100];

vector<pair<Point, Point>> linii;
vector<pair<Point, Point>> pozlinii;

int di[5] = { 0, 1, 0, -1 };
int dj[5] = { 1, 0, -1, 0 };
int dip[5] = { 0, 2, 0, -2 };
int djp[5] = { 2, 0, -2, 0 };

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

vector<muchie> spanningTree1;
vector<muchie> spanningTree2;

void cost_sort(int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (g[i].cost > g[j].cost)
            {
                int aux = g[i].cost;
                g[i].cost = g[j].cost;
                g[j].cost = aux;

                aux = g[i].x;
                g[i].x = g[j].x;
                g[j].x = aux;

                aux = g[i].y;
                g[i].y = g[j].y;
                g[j].y = aux;
            }
        }
    }
}

vector<muchie> kruskal(int n, int m)
{
    int k, tata[100], w = 0;
    vector<muchie> mst;
    cost_sort(m);
    for (int i = 1; i <= n; i++)
    {
        tata[i] = i;
    }
    for (int i = 1; i <= m; i++)
    {
        if (tata[g[i].x] != tata[g[i].y])
        {
            w += g[i].cost;
            mst.push_back(g[i]);
            k = tata[g[i].y];
            for (int j = 1; j <= n; j++)
            {
                if (tata[j] == k)
                {
                    tata[j] = tata[g[i].x];
                }
            }
        }
    }
    return mst;
}


pair<Point, Point> botMove(int player, int mapsize) 
{
    for (const auto& edge : spanningTree1) 
    {
        Point p1 = { edge.x / mapsize, edge.x % mapsize };
        Point p2 = { edge.y / mapsize, edge.y % mapsize };
        if (!exista(p1, p2) && map[p1.y][p1.x] == player && map[p2.y][p2.x] == player) 
        {
            for (const auto& altEdge : spanningTree2) 
            {
                Point altP1 = { altEdge.x / mapsize, altEdge.x % mapsize };
                Point altP2 = { altEdge.y / mapsize, altEdge.y % mapsize };
                if (!exista(altP1, altP2) && map[altP1.y][altP1.x] == player && map[altP2.y][altP2.x] == player) 
                {
                    return { altP1, altP2 };
                }
            }
        }
    }
    return { { -1, -1 }, { -1, -1 } };
}

void createEdgeDisjointSpanningTrees(int mapsize) 
{
    spanningTree1 = kruskal(mapsize, mapsize * mapsize);

    set<pair<int, int>> edgesInFirstTree;
    for (const auto& edge : spanningTree1) 
    {
        edgesInFirstTree.insert({ min(edge.x, edge.y), max(edge.x, edge.y) });
    }

    for (int i = 0; i < mapsize * mapsize; ++i) 
    {
        for (int j = i + 1; j < mapsize * mapsize; ++j) {
            if (map[i / mapsize][i % mapsize] == map[j / mapsize][j % mapsize]) 
            {
                pair<int, int> edge = { min(i, j), max(i, j) };
                if (edgesInFirstTree.find(edge) == edgesInFirstTree.end()) 
                {
                    spanningTree2.push_back({ i, j, 1 });
                    edgesInFirstTree.insert(edge);
                }
            }
        }
    }
}

void afisSpanningTrees()
{
    cout << "tree1: ";
    for (const auto& edge : spanningTree1)
    {
        cout << edge.x << " " << edge.y << " " << edge.cost << " " << '\n';
    }
    cout << "tree2: ";
    for (const auto& edge : spanningTree2)
    {
        cout << edge.x << " " << edge.y << " " << edge.cost << " " << '\n';
    }
    cout << '\n';
}

void debugmaps(int n)
{
    cout << "mapa playerului 1: " << '\n';
    afisMap(n, p1road);
    cout << "mapa playerului 2: " <<'\n';
    afisMap(n, p2road);
    cout << "Spanning Trees: " << '\n';
    afisSpanningTrees();
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
    createEdgeDisjointSpanningTrees(n);
}

int GUI(char text[], int diffx, int diffy)
{
    int ok = 0;
    int text_width = textwidth(text);
    int text_height = textheight(text);
    int tx = (sW / 2) + diffx;
    int ty = (sH / 2) + diffy;
    POINT pnt;
    GetCursorPos(&pnt);
    if (pnt.x >= tx - text_width / 2 && pnt.x <= tx + text_width / 2 && pnt.y >= ty - text_height / 2 && pnt.y <= ty + text_height / 2)
    {
        settextstyle(font, direction, hover_font_size);
        if (ismouseclick(WM_LBUTTONDOWN))
        {
            ok=1;
            clearmouseclick(WM_LBUTTONDOWN);
        }
    }
    else
    {
        settextstyle(font, direction, font_size);
    }
    outtextxy(tx - text_width / 2, ty - text_height / 2, text);
    settextstyle(font, direction, font_size);
    return ok;
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
                setbkcolor(COLOR(16, 16, 16));
                delay(100);
                break;
            case 0:
                closegraph();
                initwindow(sW, sH, "", -3, -3);
                fullscreen = 1;
                settextstyle(font, direction, font_size);
                setbkcolor(COLOR(16, 16, 16));
                delay(100);
                break;
        }
    }
}

void drawMap(int mapsize, int epsilon, int r, int g, int b)
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
                setfillstyle(SOLID_FILL, COLOR(r, g, b));
                floodfill(circleX, circleY, RED);
            }
            else if (map[i][j] == -1)
            {
                setcolor(BLUE);
                circle(circleX, circleY, radius);
                setfillstyle(SOLID_FILL, COLOR(b, g, r));
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

int wincon(int player, int mapsize)
{
    int win = 0;
    if(player == 1)
    {
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
        for (int i = 0; i < mapsize; i++)
        {
            if (p2road[i][0] == 1)
            {
                fill(i, 0, mapsize, 0, p2road, win, player);
            }
        }
    }
    return win;
}

int eval(int player, int mapsize) 
{
    int win = wincon(player, mapsize);
    if (win == 1) return 1000;
    if (win == 2) return -1000;
    return 0;
}

bool apartine(int i, int j, int n, int m)
{
    return i >= 1 && j >= 1 && i <= n && j <= m;
}

int main()
{
    int R=0, G=0, B=0;
    //65, 65, 202
    srand(time(NULL));
    initwindow(sW, sH, "", -3, -3);

    int activePage = 0;

    setbkcolor(COLOR(16, 16, 16));

    int AP = 0, GM = 0, RV = 0;

    while (1)
    {
        Fullscreen();

        setactivepage(activePage);
        setvisualpage(1 - activePage);
        cleardevice();

        int PVP = 0;
        int PVAI = 0;
        int BT = 0;
        int PC = 0;

        PVP = GUI("Player V.S. Player", -400, 0);
        PVAI = GUI("Player V.S. AI", 400, 0);
        PC = GUI("Player Color", -400, 200);
        BT = GUI("Bot Difficulty", 400, 200);

        settextstyle(5, 0, sH/100);
        outtextxy(sW / 2 - textwidth("Bridg-it")/2, sH / 6, "Bridg-it");
        settextstyle(font, direction, font_size);

        int numb[5];
        char ch[5];

        if (PC == 1)
        {
            setactivepage(0);
            setvisualpage(0);
            cleardevice();

            while (1)
            {
                setactivepage(activePage);
                setvisualpage(1 - activePage);
                cleardevice();

                AP = GUI("Albastru & Portocaliu", 0, 0);
                GM = GUI("Turcoaz & Verde", 0, -200);
                RV = GUI("Mov & Roz", 0, 200);

                if ((GetKeyState(VK_ESCAPE) & 0x8000) || AP || GM || RV)
                {
                    break;
                }
                delay(25);
                activePage = 1 - activePage;
            }
        }
        R = 255; G = 153, B = 51;
        if (AP)
        {
            R = 255; G = 153, B = 51;
        }
        if (GM)
        {
            R = 200; G = 255; B = 16;
        }
        if (RV)
        {
            R = 255; G = 105, B = 102;
        }


        if (BT == 1)
        {
            setactivepage(0);
            setvisualpage(0);
            cleardevice();

            while (1)
            {
                char txt[100] = { 'B', 'o', 't', ' ', 'D', 'i', 'f', 'f', 'i', 'c', 'u', 'l', 't', 'y', ':', ' '};
                outtextxy(sW / 2 - textwidth(txt) , sH / 2, txt);
                do
                {
                    int ok = 0;
                    char aux;
                    int ch_width;
                    int text_width = textwidth(txt);
                    ch[0] = BotDifficulty + '0';
                    ch[1] = '\0';
                    ch_width = textwidth(ch);
                    outtextxy(sW / 2 + ch_width * 2, sH / 2, ch);
                    aux = getch();
                    if (aux >= 48 && aux <= 57)
                    {
                        ok++;
                        numb[0] = (int)aux;
                        ch[0] = (int)aux;
                        ch[1] = '\0';
                        ch_width = textwidth(ch);
                        outtextxy(sW / 2 + ch_width * 2, sH / 2, ch);
                    }
                    if (ok == 1)
                    {
                        char key = getch();
                        if (key < 48 || key > 57)
                        {
                            BotDifficulty = charNr_to_int(ch);
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
            }
        }

        if (PVP == 1 || PVAI == 1)
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
                            if (mapsize % 2 == 0)
                            {
                                outtextxy(sW / 2 - textwidth("Please select an odd number")/2, sH/ 2 + textheight("Please select an odd number")*2, "Please select an odd number");
                            }
                            else
                            {
                                break;
                            }
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
                int botwin = 0;
                int mutare = 0;

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

                    drawMap(mapsize, epsilon, R, G, B);

                    int playercolor = -1;
                    for (int i = 0; i < linii.size(); i++)
                    {
                        const auto& linePair = linii[i];
                        if (playercolor == 1)
                        {
                            setcolor(COLOR(B, G, R));
                            playercolor = -playercolor;
                            
                        }
						else if (playercolor == -1)
                        {
                            setcolor(COLOR(R, G, B));
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
                                                mutare = 1;
                                                if (player == 1)
                                                {
                                                    p1road[firstClick.y][firstClick.x] = 1;
                                                    p1road[secondClick.y][secondClick.x] = 1;
                                                    if (firstClick.y == secondClick.y && firstClick.x != secondClick.x)
                                                    {

                                                        p1road[firstClick.y][max(firstClick.x, secondClick.x) - 1] = 1;
                                                    }
                                                    if (firstClick.y != secondClick.y && firstClick.x == secondClick.x)
                                                    {

                                                        p1road[max(firstClick.y, secondClick.y) - 1][firstClick.x] = 1;
                                                    }
                                                }
                                                if (player == -1 && PVAI == 0)
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
                                                }
                                                if (PVAI == 0)
                                                {
                                                    win = wincon(player, mapsize);
                                                }
                                                firstClick = { -1, -1 };
                                                if (PVAI == 0)
                                                {
                                                    player = -player;
                                                }
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
                        if (PVAI == 1 && BotDifficulty == 2 && mutare == 1)
                        {
                            pair<Point, Point> botlinie = botMove(-player, mapsize);
                            //cout << botlinie.first.x << " " << botlinie.first.y << " " << botlinie.second.x << " " << botlinie.second.y << '\n';
                            if (botlinie.first.x != -1 && botlinie.first.y != -1)
                            {
                                pozlinii.push_back(botlinie);
                                linii.push_back({ {sW / 2 - mapsize / 2 * epsilon + botlinie.first.x * epsilon, sH / 2 - mapsize / 2 * epsilon + botlinie.first.y * epsilon},
                                                  {sW / 2 - mapsize / 2 * epsilon + botlinie.second.x * epsilon, sH / 2 - mapsize / 2 * epsilon + botlinie.second.y * epsilon} });
                                win = wincon(-player, mapsize);
                                if (win == 2)
                                {
                                    botwin = 1;
                                }
                                p2road[botlinie.first.y][botlinie.first.x] = 1;
                                p2road[botlinie.second.y][botlinie.second.x] = 1;
                                if (botlinie.first.y == botlinie.second.y && botlinie.first.x != botlinie.second.x)
                                {

                                    p2road[botlinie.first.y][max(botlinie.first.x, botlinie.second.x) - 1] = 1;
                                }
                                if (botlinie.first.y != botlinie.second.y && botlinie.first.x == botlinie.second.x)
                                {

                                    p2road[max(botlinie.first.y, botlinie.second.y) - 1][botlinie.first.x] = 1;
                                }
                            }
                            mutare = 0;
                        }
                        if (PVAI == 1 && BotDifficulty == 1 && mutare == 1)
                        {
                            while (mutare == 1)
                            {    
                                int deciziei = rand() % mapsize;
                                int deciziej = rand() % mapsize;
                                int directie = rand() % 4;
                                if (apartine(deciziei, deciziej, mapsize-1, mapsize-1) && apartine(deciziei + dip[directie], deciziej + djp[directie], mapsize-1, mapsize-1))
                                {
                                    if ((abs(deciziej - deciziej + djp[directie]) == 2 && deciziei == deciziei + dip[directie]) ||
                                        (abs(deciziei - deciziei + dip[directie]) == 2 && deciziej == deciziej + djp[directie]))
                                    {
                                        if (!exista({ deciziei,deciziej }, { deciziei + dip[directie], deciziej + djp[directie] }) && map[deciziei][deciziej] == -player && map[deciziei + dip[directie]][deciziej + djp[directie]] == -player && !intersectie({ deciziej,deciziei }, { deciziej + djp[directie], deciziei + dip[directie] }, epsilon, mapsize))
                                        {
                                            //cout << deciziei << " " << deciziej << " " << deciziei + dip[directie] << " " << deciziej + djp[directie] << '\n';
                                            pozlinii.push_back({ { deciziei,deciziej },{ deciziei + dip[directie], deciziej + djp[directie] } });
                                            linii.push_back({ {sW / 2 - mapsize / 2 * epsilon + deciziej * epsilon, sH / 2 - mapsize / 2 * epsilon + deciziei * epsilon},
                                                              {sW / 2 - mapsize / 2 * epsilon + (deciziej + djp[directie]) * epsilon, sH / 2 - mapsize / 2 * epsilon + (deciziei + dip[directie]) * epsilon} });
                                            mutare = 0;
                                        }
                                    }
                                }
                            }
                        }
                        //debugmaps(mapsize);
                        if (botwin == 0 && PVAI)
                        {
                            win = wincon(player, mapsize);
                        }
                        switch (win)
                        {
                            case 1:
                                setactivepage(0);
                                setvisualpage(0);
                                cleardevice();
                                setcolor(COLOR(R, G, B));
                                outtextxy(sW / 2 - textwidth("Player 1 wins!") / 2, sH / 2, "Player 1 wins!");
                                clearmouseclick(WM_LBUTTONDOWN);
                                while (1)
                                {
                                    if (GetKeyState(VK_ESCAPE) & 0x8000)
                                    {
                                        setcolor(COLOR(R, G, B));
                                        break;
                                    }
                                }
                                break;
                            case 2:
                                setactivepage(0);
                                setvisualpage(0);
                                cleardevice();
                                setcolor(COLOR(B, G, R));
                                outtextxy(sW / 2 - textwidth("Player 2 wins!") / 2, sH / 2, "Player 2 wins!");
                                clearmouseclick(WM_LBUTTONDOWN);
                                while (1)
                                {
                                    if (GetKeyState(VK_ESCAPE) & 0x8000)
                                    {
                                        setcolor(COLOR(R, G, B));
                                        break;
                                    }
                                }
                                break;
                        }
                        linepage = 1 - linepage;
                    }

                    if (PVAI == 0)
                    {
                        if (player == -1)
                        {
                            setcolor(COLOR(B, G, R));
                            outtextxy(sW / 8 - textwidth("Turn: Player 2") / 2, sH / 2, "Turn: Player 2");
                            setcolor(COLOR(R, G, B));
                        }
                        if (player == 1)
                        {
                            setcolor(COLOR(R, G, B));
                            outtextxy(sW / 8 - textwidth("Turn: Player 1") / 2, sH / 2, "Turn: Player 1");
                            setcolor(COLOR(B, G, R));
                        }
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
        delay(25);
        activePage = 1 - activePage;

    }
    return 0;
}
