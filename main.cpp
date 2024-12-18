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
vector<vector<int>> botar1;
vector<vector<int>> botar2;
vector<vector<int>> botaux;

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

bool apartine(int i, int j, int n, int m)
{
    return i >= 1 && j >= 1 && i <= n && j <= m;
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

void debugmaps(int n)
{
    cout << "mapa playerului 1: " << '\n';
    afisMap(n, p1road);
    cout << "mapa playerului 2: " << '\n';
    afisMap(n, p2road);
}

void fillwin(int istart, int jstart, int n, int v, vector<vector <int>> a, int& win, int player)
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
            if (iv >= 0 && iv < n && jv >= 0 && jv < n && a[iv][jv] == 1 && a[iv][jv] != v)
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

    //debugmaps(n);
}

void fillbot(int istart, int jstart, int n, int v)
{
    queue<pair<int, int>> Q;
    Q.push(make_pair(istart, jstart));
    botaux[istart][jstart] = v;
    while (!Q.empty())
    {
        int i = Q.front().first, j = Q.front().second;
        for (int k = 0; k < 4; k++)
        {
            int iv = i + di[k], jv = j + dj[k];
            if (iv >= 0 && iv < n && jv >= 0 && jv < n && botaux[iv][jv] >= 1 && botaux[iv][jv] != v)
            {
                botaux[iv][jv] = v;
                Q.push(make_pair(iv, jv));
            }
        }
        Q.pop();
    }
}

void genbotar1(int n)
{
    for (int i = 1; i < n - 1; i++)
    {
        for (int j = 1; j < n - 1; j++)
        {
            if (j % 2 == 1 && j + i >= n && botar1[i][j] == 0)
            {
                if (apartine(i + 1, j, n, n) && apartine(i - 1, j, n, n))
                {
                    if (botar1[i - 1][j] == 1 && botar1[i + 1][j] == 1)
                    {
                        botar1[i][j] = 1;
                    }
                }
            }
            if (j % 2 == 0 && j + i < n && botar1[i][j] == 0)
            {
                if (apartine(i, j + 1, n, n) && apartine(i, j - 1, n, n))
                {
                    if (botar1[i][j - 1] == 1 && botar1[i][j + 1] == 1)
                    {
                        botar1[i][j] = 1;
                    }
                }
            }
            if (i + j == n - 1)
            {
                botar1[i][j] = 1 - botar1[i][j];
            }
        }
    }
    for (int i = 1; i < n - 1; i++)
    {
        botar1[0][i] = 2;
        botar1[n - 1][i] = 2;
    }
    /*for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << setw(5) << botar1[i][j];
        }
        cout << '\n';
    }
    cout << '\n';*/
}

void genbotar2(int n)
{
    for (int i = 1; i < n - 1; i++)
    {
        for (int j = 1; j < n - 1; j++)
        {
            if (j % 2 == 1 && j + i >= n && botar2[i][j] == 0)
            {
                if (apartine(i + 1, j, n, n) && apartine(i - 1, j, n, n))
                {
                    if (botar2[i - 1][j] == 1 && botar2[i + 1][j] == 1)
                    {
                        botar2[i][j] = 1;
                    }
                }
            }
            if (j % 2 == 0 && j + i < n && botar2[i][j] == 0)
            {
                if (apartine(i, j + 1, n, n) && apartine(i, j - 1, n, n))
                {
                    if (botar2[i][j - 1] == 1 && botar2[i][j + 1] == 1)
                    {
                        botar2[i][j] = 1;
                    }
                }
            }
        }
    }
    for (int i = 1; i < n - 1; i++)
    {
        for (int j = 1; j < n - 1; j++)
        {
            botaux[i][j] = botar2[j][i];
        }
    }
    for (int i = 1; i < n - 1; i++)
    {
        for (int j = 1; j < n - 1; j++)
        {
            botar2[i - 1][j - 1] = botaux[i][j];
        }
    }
    for (int i = 1; i < n - 1; i++)
    {
        botar2[0][i] = 2;
        botar2[n - 1][i] = 2;
    }
    for (int i = 1; i < n - 1; i++)
    {
        if (i % 2 != 0)
        {
            botar2[i][n - 2] = 0;
            botar2[n - 2][i] = 0;
        }
    }
    /*for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << setw(5) << botar2[i][j];
        }
        cout << '\n';
    }
    cout << '\n';*/
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
                botar1[i][j] = 1;
                botar2[i][j] = 1;
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
    genbotar1(n);
    genbotar2(n);
    cout << '\n';
}

int intersectie(Point p1, Point p2, int epsilon, int mapsize)
{
    Point tp1;
    Point tp2;
    if (p1.x == p2.x)
    {
        if (p1.y != p2.y)
        {
            tp1 = { p1.x - 1, (p1.y + p2.y) / 2 };
            tp2 = { p2.x + 1, (p1.y + p2.y) / 2 };
        }
    }
    else if (p1.y == p2.y)
    {
        if (p1.x != p2.x)
        {
            tp1 = { (p1.x + p2.x) / 2, p1.y - 1 };
            tp2 = { (p1.x + p2.x) / 2, p2.y + 1 };
        }
    }
    if (exista(tp1, tp2))
    {
        return 1;
    }
    return 0;
}

pair<Point, Point> botmove(int n, int epsilon, int player)
{
    Point fc;
    Point sc;
    int arbore1 = 0, arbore2 = 0, auxi, auxj;
    for (int i = 0; i < n; i++)
    {
        for (int j = 1; j < n - 1; j++)
        {
            botaux[i][j] = botar1[i][j];
        }
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 1; j < n - 1; j++)
        {
            if (botaux[i][j] >= 1)
            {
                arbore1++;
                if (arbore1 == 2)
                {
                    auxi = i;
                    auxj = j;
                }
                fillbot(i, j, n, 0);
            }
        }
    }
    if (arbore1 == 2)
    {
        queue<pair<int, int>> Q;
        Q.push(make_pair(auxi, auxj));
        int ok = 0;
        while (!Q.empty())
        {
            int i = Q.front().first;
            int j = Q.front().second;
            for (int k = 0; k <= 3; k++)
            {
                int ik = i + di[k];
                int jk = j + dj[k];
                int ikp = i + dip[k];
                int jkp = j + djp[k];

                pair<Point, Point> pereche = { {j,i}, {j + djp[k],i + dip[k]} };
                if (apartine(pereche.first.y, pereche.first.x, n - 1, n - 1) && apartine(pereche.second.y, pereche.second.x, n - 1, n - 1))
                {
                    if ((abs(pereche.first.x - pereche.second.x) == 2 && pereche.first.y == pereche.second.y) ||
                        (abs(pereche.first.y - pereche.second.y) == 2 && pereche.first.x == pereche.second.x))
                    {
                        if (!exista(pereche.first, pereche.second) && map[pereche.first.y][pereche.first.x] == -player && map[pereche.second.y][pereche.second.x] == -player && !intersectie(pereche.first, pereche.second, epsilon, n))
                        {
                            if (botar2[ik][jk] == 1 && botar2[ikp][jkp] == 1 && botar1[ik][jk] != 1)
                            {
                                botar1[ik][jk] = 2;
                                botar1[ikp][jkp] = 2;
                                botar2[ik][jk] = 2;
                                botar2[ikp][jkp] = 2;
                                botar1[i][j] = 2;
                                botar2[i][j] = 2;
                                fc = { j,i };
                                sc = { jkp, ikp };
                                ok = 1;
                                break;
                            }
                        }
                    }
                }
            }
            for (int k = 0; k <= 3; k++)
            {
                int iv = i + di[k], jv = j + dj[k];
                if (iv >= 0 && iv < n && jv >= 0 && jv < n && botar1[iv][jv] >= 1)
                {
                    Q.push(make_pair(iv, jv));
                }
            }
            if (ok)
            {
                break;
            }
            Q.pop();
        }
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 1; j < n - 1; j++)
        {
            botaux[i][j] = botar2[i][j];
        }
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 1; j < n - 1; j++)
        {
            if (botaux[i][j] >= 1)
            {
                arbore2++;
                if (arbore2 == 2)
                {
                    auxi = i;
                    auxj = j;
                }
                fillbot(i, j, n, 0);
            }
        }
    }
    if (arbore2 == 2)
    {
        queue<pair<int, int>> Q;
        Q.push(make_pair(auxi, auxj));
        int ok = 0;
        while (!Q.empty())
        {
            int i = Q.front().first;
            int j = Q.front().second;
            for (int k = 0; k <= 3; k++)
            {
                int ik = i + di[k];
                int jk = j + dj[k];
                int ikp = i + dip[k];
                int jkp = j + djp[k];

                pair<Point, Point> pereche = { {j,i}, {j + djp[k],i + dip[k]} };
                if (apartine(pereche.first.y, pereche.first.x, n - 1, n - 1) && apartine(pereche.second.y, pereche.second.x, n - 1, n - 1))
                {
                    if ((abs(pereche.first.x - pereche.second.x) == 2 && pereche.first.y == pereche.second.y) ||
                        (abs(pereche.first.y - pereche.second.y) == 2 && pereche.first.x == pereche.second.x))
                    {
                        if (!exista(pereche.first, pereche.second) && map[pereche.first.y][pereche.first.x] == -player && map[pereche.second.y][pereche.second.x] == -player && !intersectie(pereche.first, pereche.second, epsilon, n))
                        {
                            if (botar1[ik][jk] == 1 && botar1[ikp][jkp] == 1 && botar2[ik][jk] != 1)
                            {
                                botar1[ik][jk] = 2;
                                botar1[ikp][jkp] = 2;
                                botar2[ik][jk] = 2;
                                botar2[ikp][jkp] = 2;
                                botar1[i][j] = 2;
                                botar2[i][j] = 2;
                                fc = { j,i };
                                sc = { jkp, ikp };
                                ok = 1;
                                break;
                            }
                        }
                    }
                }
            }
            for (int k = 0; k <= 3; k++)
            {
                int iv = i + di[k], jv = j + dj[k];
                if (iv >= 0 && iv < n && jv >= 0 && jv < n && botar2[iv][jv] >= 1)
                {
                    Q.push(make_pair(iv, jv));
                }
            }
            if (ok)
            {
                break;
            }
            Q.pop();
        }
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << setw(5) << botar1[i][j];
        }
        cout << '\n';
    }
    cout << '\n';
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << setw(5) << botar2[i][j];
        }
        cout << '\n';
    }
    cout << '\n';
    cout << arbore1 << " " << arbore2 << '\n';
    return { fc, sc };
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
            ok = 1;
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

int wincon(int player, int mapsize)
{
    int win = 0;
    if (player == 1)
    {
        for (int i = 0; i < mapsize; i++)
        {
            if (p1road[0][i] == 1)
            {
                fillwin(0, i, mapsize, 0, p1road, win, player);
            }
        }
    }
    if (player == -1)
    {
        for (int i = 0; i < mapsize; i++)
        {
            if (p2road[i][0] == 1)
            {
                fillwin(i, 0, mapsize, 0, p2road, win, player);
            }
        }
    }
    return win;
}

int eval(int player, int mapsize)
{
    int win = wincon(player, mapsize);
    return 0;
}

int main()
{
    int R = 0, G = 0, B = 0;
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

        settextstyle(5, 0, sH / 100);
        outtextxy(sW / 2 - textwidth("Bridg-it") / 2, sH / 6, "Bridg-it");
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
                char txt[100] = { 'B', 'o', 't', ' ', 'D', 'i', 'f', 'f', 'i', 'c', 'u', 'l', 't', 'y', ':', ' ' };
                outtextxy(sW / 2 - textwidth(txt), sH / 2, txt);
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
                                outtextxy(sW / 2 - textwidth("Please select an odd number") / 2, sH / 2 + textheight("Please select an odd number") * 2, "Please select an odd number");
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

                botar1.clear();
                botar2.clear();

                botar1.resize(mapsize + 1);
                for (int i = 0; i < mapsize; i++)
                {
                    botar1[i].resize(mapsize);
                }

                botar2.resize(mapsize + 1);
                for (int i = 0; i < mapsize; i++)
                {
                    botar2[i].resize(mapsize);
                }

                botaux.resize(mapsize + 1);
                for (int i = 0; i < mapsize; i++)
                {
                    botaux[i].resize(mapsize);
                }

                genMap(mapsize);

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

                if (PVAI)
                {
                    player = -player;
                    mutare = 1;
                }

                while (1)
                {
                    int epsilon = sH / 10;

                    drawMap(mapsize, epsilon, R, G, B);

                    int playercolor = -1;

                    if (PVAI == 1 && BotDifficulty == 2 && mutare == 1)
                    {
                        pair<Point, Point> pereche = botmove(mapsize, epsilon, player);
                        cout << pereche.first.x << " " << pereche.first.y << " " << pereche.second.x << " " << pereche.second.y << '\n';
                        pozlinii.push_back(pereche);
                        linii.push_back({ {sW / 2 - mapsize / 2 * epsilon + pereche.first.x * epsilon, sH / 2 - mapsize / 2 * epsilon + pereche.first.y * epsilon},
                                          {sW / 2 - mapsize / 2 * epsilon + pereche.second.x * epsilon, sH / 2 - mapsize / 2 * epsilon + pereche.second.y * epsilon} });
                        p1road[pereche.first.y][pereche.first.x] = 1;
                        p1road[pereche.second.y][pereche.second.x] = 1;
                        if (pereche.first.y == pereche.second.y && pereche.first.x != pereche.second.x)
                        {
                            p1road[pereche.first.y][max(pereche.first.x, pereche.second.x) - 1] = 1;
                        }
                        if (pereche.first.y != pereche.second.y && pereche.first.x == pereche.second.x)
                        {
                            p1road[max(pereche.first.y, pereche.second.y) - 1][pereche.first.x] = 1;
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
                            if (apartine(deciziei, deciziej, mapsize - 1, mapsize - 1) && apartine(deciziei + dip[directie], deciziej + djp[directie], mapsize - 1, mapsize - 1))
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
                                        p1road[deciziei][deciziej] = 1;
                                        p1road[deciziei + dip[directie]][deciziej + djp[directie]] = 1;
                                        if (deciziei == deciziei + dip[directie] && deciziej != deciziej + djp[directie])
                                        {

                                            p1road[deciziei][max(deciziej, deciziej + djp[directie]) - 1] = 1;
                                        }
                                        if (deciziei != deciziei + dip[directie] && deciziej == deciziej + djp[directie])
                                        {

                                            p1road[max(deciziei, deciziei + dip[directie]) - 1][deciziej] = 1;
                                        }
                                    }
                                }
                            }
                        }
                    }

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

                        if (mutare == 0)
                        {
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
                                                    if (PVAI)
                                                    {
                                                        p2road[firstClick.y][firstClick.x] = 1;
                                                        p2road[secondClick.y][secondClick.x] = 1;

                                                        botar1[firstClick.y][firstClick.x] = 0;
                                                        botar1[secondClick.y][secondClick.x] = 0;
                                                        botar2[secondClick.y][secondClick.x] = 0;
                                                        botar2[firstClick.y][firstClick.x] = 0;

                                                        if (firstClick.y == secondClick.y && firstClick.x != secondClick.x)
                                                        {

                                                            p2road[firstClick.y][max(firstClick.x, secondClick.x) - 1] = 1;
                                                            botar1[firstClick.y][max(firstClick.x, secondClick.x) - 1] = 0;
                                                            botar2[firstClick.y][max(firstClick.x, secondClick.x) - 1] = 0;
                                                        }
                                                        if (firstClick.y != secondClick.y && firstClick.x == secondClick.x)
                                                        {

                                                            p2road[max(firstClick.y, secondClick.y) - 1][firstClick.x] = 1;
                                                            botar1[max(firstClick.y, secondClick.y) - 1][firstClick.x] = 0;
                                                            botar2[max(firstClick.y, secondClick.y) - 1][firstClick.x] = 0;
                                                        }
                                                        mutare = 1;
                                                    }
                                                    if (player == 1 && PVAI == 0)
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
                                                    firstClick = { -1, -1 };
                                                    if (PVAI == 0)
                                                    {
                                                        win = wincon(player, mapsize);
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
                        }
                        if (PVAI)
                        {
                            for (int i = 0; i < mapsize; i++)
                            {
                                if (p1road[0][i] == 1)
                                {
                                    fillwin(0, i, mapsize, 0, p1road, botwin, 1);
                                }
                            }
                            for (int i = 0; i < mapsize; i++)
                            {
                                if (p2road[i][0] == 1)
                                {
                                    fillwin(0, i, mapsize, 0, p2road, win, -1);
                                }
                            }
                            if (win == 2)
                            {
                                win = 2;
                            }
                            if (botwin == 1)
                            {
                                win = 1;
                            }
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