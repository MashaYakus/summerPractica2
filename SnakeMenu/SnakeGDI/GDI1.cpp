#include <windows.h>
#include <wingdi.h>
#include <time.h>
#include "resource.h"

#include "common.hpp"

#define LENGTH 20
#define WIDTH 20
#define pix 30
#define GREEN RGB(0,255,0)
#define RED RGB(255,0,0)
#define BLUE RGB(0,0,255)
#define BACKGRAUND RGB(128,128,128)
#define WALLS RGB(255,255,255)
#define XPICTURE pix / 3 + pix + pix / 10
#define YPICTURE pix + pix / 7
#define SZSCORE		"SCORE:"


#define YMK_TMR_				1000
#define YMK_TMR_SNAKE				(YMK_TMR_ + 0x01)

void FruitRand (TSnake& snake) {
    srand((int)time(NULL));
    snake.fruit = {rand() % (LENGTH - 1) + 1, rand() % (WIDTH - 1) + 1};
}

void Start (TSnake& snake) {
    srand((int)time(NULL));
	if (snake.score != 0){
		snake.tail.Nul();
		snake.tail.Capacity = 0;
	}
    snake.gameOver = false;
    snake.state = Stop;
    snake.score = 0;
	snake.pause = Stop;
    snake.head = {WIDTH / 2, LENGTH / 2};
    FruitRand(snake);
	//snake.fruit = {WIDTH / 2, LENGTH/2};
    while (snake.fruit == snake.head)
        FruitRand(snake);
    snake.walls = false;
    snake.game = Hard;
}

void Logic (TSnake& snake) {
    int i;
    if (snake.state != Stop){
        if (snake.score > 0 && snake.state != Stop){
            snake.pastTeil = snake.tail[0];
            for (i = 0; i < snake.score - 1; i++){
                snake.tail[i] = snake.tail[i + 1];
            }
            snake.tail[i] = snake.head;
        }
    }
    if (snake.state == Right)
        snake.head.x++;
    else if (snake.state == Left)
        snake.head.x--;
    else if (snake.state == Up)
        snake.head.y--;
    else if (snake.state == Down)
        snake.head.y++;
    else
        return;
    if (snake.walls && (snake.head.x == WIDTH + 1 ||
                        snake.head.x == 0 ||
                        snake.head.y == LENGTH ||
                        snake.head.y == 0) )
        snake.gameOver = true;
    else {
        if (snake.head.x == WIDTH)
            snake.head.x = 0;
        if (snake.head.x == -1)
            snake.head.x = WIDTH - 1;
        if (snake.head.y == LENGTH)
            snake.head.y = 0;
        if (snake.head.y == -1)
            snake.head.y = LENGTH - 1;
    }

    if (snake.head == snake.fruit) {
        snake.score++;
        snake.tail.Push();
        snake.tail[snake.score - 1] = snake.head;
        FruitRand(snake);
        for (i = 0; i < snake.score - 1; i++){
            if (snake.fruit == snake.tail[i] || snake.fruit == snake.head){
                FruitRand(snake);
                i = -1;
            }
        }
    }

    for (i = 0; i < snake.score - 1; i++){
        if (snake.head == snake.tail[i])
            snake.gameOver = true;
    }
}

typedef struct _ymk_picture {
    TSnake snake;
	HBRUSH hBrush, sBrush, fBrush, wBrush;
	HPEN hPen, sPen, fPen, wPen;
	bool writeText = false, clickTimer = false;
	RECT score_rect;
	HICON	sIR, sIL, sIU, sID, sIA;

} YDATA;


LRESULT		wnd_WM_CREATE (
	HWND wnd,
	CREATESTRUCT* cs)
{
	YDATA* yd = (YDATA*)cs->lpCreateParams;
	yd->hBrush = CreateSolidBrush(BACKGRAUND);
	yd->sBrush = CreateSolidBrush(GREEN);
	yd->fBrush = CreateSolidBrush(RED);
	yd->wBrush = CreateSolidBrush(WALLS);
	yd->hPen = CreatePen(PS_SOLID, 1, BACKGRAUND);
	yd->sPen = CreatePen(PS_SOLID, 1, GREEN);
	yd->fPen = CreatePen(PS_SOLID, 1, RED);
	yd->wPen = CreatePen(PS_SOLID, 1, WALLS);
	yd->sID = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_SNAKE_HLD));
	yd->sIU = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_SNAKE_HLU));
	yd->sIR = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_SNAKE_HLR));
	yd->sIL = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_SNAKE_HLL));
	yd->sIA = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_APP));
	Start(yd->snake);
	SetWindowLong(wnd,4,(LPARAM)yd);
	return (0);
}


void		wnd_WM_DESTROY (
	HWND wnd,
	YDATA* yd)
{
	DeleteObject(yd->hBrush);
	DeleteObject(yd->sBrush);
	DeleteObject(yd->fBrush);
	DeleteObject(yd->wBrush);
	DeleteObject(yd->hPen);
	DeleteObject(yd->fPen);
	DeleteObject(yd->sPen);
	DeleteObject(yd->wPen);
	DestroyIcon(yd->sIR);
	DestroyIcon(yd->sIL);
	DestroyIcon(yd->sIU);
	DestroyIcon(yd->sID);
	DestroyIcon(yd->sIA);
}

bool ClickWindows (WPARAM wParam, YDATA* yd) {
	if (yd->clickTimer)
		return true;
    switch (wParam) {
    case 0x00000026: // ARROW UP
    case 0x00000057: //W
        if (yd->snake.state != Down){
			yd->snake.state = Up;
		}
        break;
    case 0x00000028: // ARROW DOWN
    case 0x00000053: //S
        if (yd->snake.state != Up){
			yd->snake.state = Down;
		}
        break;
    case 0x00000025: // ARROW LEFT
    case 0x00000041: //A
        if (yd->snake.state != Right){
			yd->snake.state = Left;
		}
        break;
    case 0x00000027: // ARROW RIGHT
    case 0x00000044: //D
        if (yd->snake.state != Left){
		    yd->snake.state = Right;
		}
        break;
    case 0x00000050: //P
        if (yd->snake.state != Stop){
            yd->snake.pause = yd->snake.state;
            yd->snake.state = Stop;
        }
        else {
			yd->snake.state = yd->snake.pause;
		}
        break;
    case 0x00000058: //X
        yd->snake.gameOver = true;
        break;
	case 	0x1B: //Esc
		PostQuitMessage(0);
		break;
    default:
        return false;
    }
	yd->clickTimer = true;
    return true;
}

BOOL DrawLine (HDC hdc, int x1, int y1, int x2, int y2)
{
  POINT pt;
  MoveToEx(hdc, x1, y1, &pt);
  return LineTo(hdc, x2, y2);
}

void DrawHeadSnake (
	HWND wnd,
	YDATA* yd){

	HDC hDC = GetWindowDC(wnd);
	int nxLeft, nyTop, nxRight, nyBottom;
	int nxStart, nyStart, nxEnd, nyEnd = -1;
	int nxs2el, nys2el, nxe2el, nye2el;

	nxLeft = pix * yd->snake.head.x;
	nyTop = pix + pix * yd->snake.head.y;
	nxRight = pix * yd->snake.head.x + pix;
	nyBottom = pix + pix * yd->snake.head.y + pix;

	if (yd->snake.state == Right){
		nxStart = nxLeft + pix / 3;
		nyStart = nyTop;
		nxEnd = nxStart;
		nyEnd = nyBottom;

		nxs2el = nxLeft;
		nys2el = nyTop;
		nxe2el = nxRight + pix;
		nye2el = nyBottom;

	}
	else if (yd->snake.state == Left){
		nxStart = nxLeft + 2 * pix / 3;
		nyStart = nyBottom;
		nxEnd = nxStart;
		nyEnd = nyTop;

		nxs2el = nxLeft - pix;
		nys2el = nyTop;
		nxe2el = nxRight;
		nye2el = nyBottom;

	}
	else if (yd->snake.state == Up) {
		nxStart = nxLeft;
		nyStart = nyTop +  2 * pix / 3;
		nxEnd = nxRight;
		nyEnd = nyStart;

		nxs2el = nxLeft;
		nys2el = nyTop - pix;
		nxe2el = nxRight;
		nye2el = nyBottom;
	}
	else if (yd->snake.state == Down) {
		nxStart = nxRight;
		nyStart = nyTop +  pix / 3;
		nxEnd = nxLeft;
		nyEnd = nyStart;

		nxs2el = nxLeft;
		nys2el = nyTop;
		nxe2el = nxRight;
		nye2el = nyBottom + pix;
	}
	else if (nyEnd = -1) {
		nxStart = nxLeft + pix / 3;
		nyStart = nyTop;
		nxEnd = nxStart;
		nyEnd = nyBottom;

		nxs2el = nxLeft;
		nys2el = nyTop;
		nxe2el = nxRight + pix;
		nye2el = nyBottom;
	}
	HGDIOBJ	oldP, oldB;
	oldP = SelectObject(hDC, yd->sPen);
	oldB = SelectObject(hDC, yd->sBrush);
	Pie (hDC, nxs2el + XPICTURE, nys2el + YPICTURE, nxe2el + XPICTURE, nye2el + YPICTURE, nxStart + XPICTURE, nyStart + YPICTURE, nxEnd + XPICTURE, nyEnd + YPICTURE);
	SelectObject(hDC, oldP);
	SelectObject(hDC, oldB);
}

void DrawHeadSnakeIcon (
	HWND wnd,
	YDATA* yd)
{
	HDC hDC = GetWindowDC(wnd);
	int nxLeft, nyTop;
	bool fs = true;
	HICON sIH;
	nxLeft = pix * yd->snake.head.x;
	nyTop = pix + pix * yd->snake.head.y;

	if (yd->snake.state == Right){
		sIH = yd->sIR;
	}
	else if (yd->snake.state == Left){
		sIH = yd->sIL;
	}
	else if (yd->snake.state == Up){
		sIH = yd->sIU;
	}
	else if (yd->snake.state == Down) {
		sIH = yd->sID;
	}
	else if (fs){
		fs = false;
		sIH = yd->sIR;
	}
	DrawIconEx(
			hDC,
			nxLeft + XPICTURE - pix / 20,
			nyTop + YPICTURE - pix / 20,
			sIH,
			pix + pix / 10,
			pix + pix / 10,
			0, 
			yd->hBrush,
			DI_NORMAL);
}

void DrawTail (
	HWND wnd,
	YDATA* yd,
	TCoordinate tail) 
{
	HDC hDC = GetWindowDC(wnd);
	int nxLeft, nyTop, nxRight, nyBottom;
	nxLeft = pix * tail.x + pix / 6;
	nyTop = pix + pix * tail.y + pix / 6;
	nxRight = pix * tail.x + pix - pix / 6;
	nyBottom = pix + pix * tail.y + pix - pix / 6;

	HGDIOBJ	oldP, oldB;
	oldP = SelectObject(hDC,yd->sPen);
	oldB = SelectObject(hDC,yd->sBrush);
	Ellipse(hDC, nxLeft + XPICTURE, nyTop + YPICTURE, nxRight + XPICTURE, nyBottom + YPICTURE);
	SelectObject(hDC, oldP);
	SelectObject(hDC, oldB);
}

void DrawEndTail (
	HWND wnd,
	YDATA* yd,
	TCoordinate tail,
	Motion en)
{
	DrawTail(wnd, yd, tail);
}

void DrawEmpty (
	HWND wnd,
	YDATA* yd,
	TCoordinate a)
{
	HDC hDC = GetWindowDC(wnd);
	int nxLeft, nyTop, nxRight, nyBottom;
	nxLeft = pix * a.x;
	nyTop = pix + pix * a.y;
	nxRight = pix * a.x + pix;
	nyBottom = pix + pix * a.y + pix;
	HGDIOBJ	oldP, oldB;
	oldP = SelectObject(hDC,yd->hPen);
	oldB = SelectObject(hDC,yd->hBrush);
	Rectangle(hDC, nxLeft + XPICTURE, nyTop+ YPICTURE, nxRight + XPICTURE, nyBottom + YPICTURE);
	SelectObject(hDC, oldP);
	SelectObject(hDC, oldB);
}

void DrawFruit (
	HWND wnd,
	YDATA* yd) 
{
	HDC hDC = GetWindowDC(wnd);
	int nxLeft, nyTop;

	nxLeft = pix * yd->snake.fruit.x;
	nyTop = pix + pix * yd->snake.fruit.y + pix / 4;
	DrawIconEx(
			hDC,
			nxLeft + XPICTURE + pix / 5,
			nyTop + YPICTURE - pix / 5,
			yd->sIA,
			pix - pix / 5,
			pix  - pix / 5,
			0, 
			yd->hBrush,
			DI_NORMAL);
}

void DrawWalls (
	HWND wnd,
	YDATA* yd)
{
	HDC hDC = GetWindowDC(wnd);
	
	//int nxLeft, nyTop, nxRight, nyBottom;
	HGDIOBJ	oldP, oldB;
	oldP = SelectObject(hDC,yd->wPen);
	oldB = SelectObject(hDC,yd->wBrush);
	Rectangle(hDC, 0 + XPICTURE - pix, 0 + YPICTURE, (WIDTH + 2) * pix + XPICTURE - pix, pix + YPICTURE - pix / 10);
	Rectangle(hDC, 0 + XPICTURE - pix, 0 + YPICTURE, pix + XPICTURE - pix - pix / 10, (LENGTH + 1) * pix + YPICTURE + pix / 10);
	Rectangle(hDC, 0 + XPICTURE - pix, (LENGTH + 1) * pix + YPICTURE + pix / 10, (WIDTH + 2) * pix + XPICTURE - pix, (LENGTH + 2) * pix + YPICTURE);
	Rectangle(hDC, (WIDTH + 1) * pix + XPICTURE - pix + pix / 10, 0 + YPICTURE, (WIDTH + 2) * pix + XPICTURE - pix, (LENGTH + 1) * pix + YPICTURE + pix / 10);
	SelectObject(hDC, oldP);
	SelectObject(hDC, oldB);
}

void DrawScore (
	HDC hDC,
	YDATA* yd)
{
	char str[12];
	HFONT	h_font;
	LOGFONTA	lf;
	HGDIOBJ	oldP, oldB, oldF;
	RECT	r, c;

	itoa(yd->snake.score, str, 10);

	memset(&lf,0,sizeof(lf));
	lf.lfHeight = -30;
	lf.lfWeight = 900;
	//lf.lfItalic = 1;
	//strcpy(lf.lfFaceName,"Arial");
	strcpy(lf.lfFaceName,"Courier New");
	h_font = CreateFontIndirect(&lf);

	SetBkMode(hDC,TRANSPARENT);
	SetTextColor(hDC,RGB(255,255,255));
	//StretchDIBits

	oldF = SelectObject(hDC,h_font);

	oldP = SelectObject(hDC,yd->hPen);
	oldB = SelectObject(hDC,yd->hBrush);

	if ( yd->writeText == false) {
		HICON	h_icon;

		memset(&r,0,sizeof(r));
		DrawText(hDC,SZSCORE,strlen(SZSCORE),&r,DT_CALCRECT);

		yd->score_rect.left = ((WIDTH + 2) * pix - r.right) / 2;
		yd->score_rect.top = (LENGTH + 2) * pix + YPICTURE;
		yd->score_rect.right = yd->score_rect.left + r.right;
		yd->score_rect.bottom = yd->score_rect.top + 2 * r.bottom;
		DrawText(hDC, SZSCORE, strlen(SZSCORE), &yd->score_rect, DT_SINGLELINE | DT_VCENTER | DT_RIGHT);

		h_icon = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_SNAKE_HLD));
		DrawIconEx(
			hDC,
			yd->score_rect.left - 30,
			yd->score_rect.top + 20,
			h_icon,
			pix,
			pix,
			0, 
			yd->hBrush,
			DI_NORMAL);
		DestroyIcon(h_icon);
	}
	memset(&c,0,sizeof(c));
	DrawText(hDC,"WWW",3,&c,DT_CALCRECT);

	r.left = yd->score_rect.right + pix / 4;
	r.top = yd->score_rect.top;
	r.right = r.left + c.right;
	r.bottom = yd->score_rect.bottom;

	Rectangle(hDC, r.left, r.top, r.right, r.bottom);
	DrawText(hDC, str, strlen(str), &r, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

	SelectObject(hDC, oldP);
	SelectObject(hDC, oldB);
	SelectObject(hDC, oldF);
	DeleteObject(h_font);
}

DWORD paint_WM_PAINT (
    HWND wnd,
    YDATA* yd)
{
    PAINTSTRUCT ps;
    RECT r;
	TCoordinate t = {1, 1};
	TCoordinate a;
    HBRUSH h_brush;
	HDC hDC = GetWindowDC(wnd);
    BeginPaint(wnd, &ps);
	for (int i = -1; i < WIDTH + 3; i++){
		for (int j = -1; j < LENGTH + 6; j++){
			a = {j, i};
			DrawEmpty(wnd, yd, a);
		}
	}
    SetRect(&r, 0, 0, (WIDTH + 4) * pix, (LENGTH + 6) * pix);
    h_brush = yd->hBrush;
    FillRect(ps.hdc, &r, h_brush);
	DrawWalls(wnd, yd);
	DrawFruit(wnd, yd);
	DrawHeadSnakeIcon(wnd, yd);
	DrawScore(hDC, yd);
	yd->writeText = true;
	//DrawTail(wnd, yd, t);
	//DrawEmpty(wnd, yd, t);
	//DrawLine(hDC, 40, 60, 120, 120);
    EndPaint(wnd, &ps);
	return 0;
}

DWORD paint_TIMER(
	HWND wnd,
    YDATA* yd)
{
	HDC hDC = GetWindowDC(wnd);

	TCoordinate a = yd->snake.head;
	if (yd->snake.state == Stop)
		return 0;
	DrawEmpty(wnd, yd, yd->snake.head);
	DrawHeadSnakeIcon(wnd, yd);
	//DrawHeadSnake(wnd, yd);
	if (yd->snake.state == Right){
        if (yd->snake.head.x == 0)
            a.x = (WIDTH - 1);
        else
            a.x--;
    }
	else if (yd->snake.state == Left){
		if (yd->snake.head.x == WIDTH - 1)
            a.x = 0;
        else
            a.x++;
	}
	else if (yd->snake.state == Up){
        if (yd->snake.head.y == LENGTH - 1)
            a.y = 0;
        else
            a.y++;
    }
    else if (yd->snake.state == Down){
        if (yd->snake.head.y == 0)
            a.y = LENGTH - 1;
        else
            a.y--;
    }
	
	if (yd->snake.score == 0 || (yd->snake.score == 1 && yd->snake.pastTeil.x == -1))
		DrawEmpty(wnd, yd, a);
	else {
		DrawEmpty(wnd, yd, a);
		DrawTail(wnd, yd, a);
		if (!(yd->snake.pastTeil == yd->snake.head))
			DrawEmpty(wnd, yd, yd->snake.pastTeil);
	}
	DrawFruit(wnd, yd);
	DrawScore(hDC, yd);
	return 0;
}

DWORD wnd_WM_COMMAND(
	HWND wnd,
    YDATA* yd,
	DWORD notify_code,
	DWORD identification,
	HWND control)
{
	DWORD	rc=0;
	if ( notify_code == 0 ) switch ( identification ) {
	case ID_PLAY_RESTART:
		Start(yd->snake);
		yd->writeText = false;
		paint_WM_PAINT(wnd, yd);
		break;
	case ID_PLAY_STOP:
		yd->snake.pause = yd->snake.state;
		yd->snake.state = Stop;
		break;
	case ID_PLAY_EXIT:
		PostQuitMessage(0);
		break;
	case ID_SETTING_WALLS:
		if (yd->snake.walls)
			yd->snake.walls = false;
		else
			yd->snake.walls = true;
		break;
	case ID_DIFFICULT_EASY:
		KillTimer(wnd, YMK_TMR_SNAKE);
		SetTimer(wnd,
                 YMK_TMR_SNAKE,
                 600,
                 (TIMERPROC)NULL);
		break;
	case ID_PLAY_MEDIUM:
		KillTimer(wnd, YMK_TMR_SNAKE);
		SetTimer(wnd,
                 YMK_TMR_SNAKE,
                 300,
                 (TIMERPROC)NULL);
		break;
	case ID_DIFFICULT_HARD:
		KillTimer(wnd, YMK_TMR_SNAKE);
		SetTimer(wnd,
                 YMK_TMR_SNAKE,
                 100,
                 (TIMERPROC)NULL);
		break;
	}
	else
		rc = 1;
	return rc;
}

LRESULT CALLBACK SnakeWindowsProc (
    HWND wnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    YDATA *yd;
    LRESULT rc;
    TSnake snake;
	RECT r;
	SetRect(&r, 0, 0,(WIDTH + 3) * pix, (LENGTH + 5) * pix);
	if (msg == WM_CREATE)
		wnd_WM_CREATE(wnd,(CREATESTRUCT*)lParam);

	yd = (YDATA*)GetWindowLong(wnd,4);
	if (yd == NULL)
		rc = DefWindowProc(wnd,msg,wParam,lParam);
	else switch(msg){
    case WM_CREATE:
        SetTimer(wnd,
                 YMK_TMR_SNAKE,
                 400,
                 (TIMERPROC)NULL);
        rc = 0;
        break;
    case WM_TIMER:
		rc = 0;
		//InvalidateRect(wnd, &r, false);
		yd->clickTimer = false;
		if (wParam == YMK_TMR_SNAKE) {
			Logic(yd->snake);
			paint_TIMER(wnd, yd);
			if (yd->snake.gameOver)
				PostQuitMessage(0);
		}
        break;
    case WM_PAINT:
		rc = 0;
        paint_WM_PAINT(wnd, yd);
        break;
    case WM_KEYDOWN:
        rc = 0;
        ClickWindows(wParam, yd);
        break;
	case WM_CLOSE:
		rc = DefWindowProc(wnd,msg,wParam,lParam);
		PostQuitMessage(0);
		break;
    case WM_DESTROY:
		rc = 0;
		wnd_WM_DESTROY(wnd, yd);
        KillTimer(wnd, YMK_TMR_SNAKE);
		//PostQuitMessage(0);
        break;
	case WM_COMMAND:
		rc = wnd_WM_COMMAND(wnd,yd,HIWORD(wParam),LOWORD(wParam),(HWND)lParam);
	    break;
    default:
        rc = DefWindowProc(wnd, msg, wParam, lParam);
    }
    return rc;
}

DWORD viewCreateWindows() {
    YDATA yd;
    int rc = 0;
    WNDCLASS wc = {0};
    HINSTANCE hins = GetModuleHandle(NULL);
    HWND wnd = NULL;
    MSG msg;
	HMENU hMenu;
    wc.style = CS_OWNDC; //стиль
    wc.lpfnWndProc = SnakeWindowsProc;
    wc.cbClsExtra = 0;   //сколько дополнительно места для класса
    wc.cbWndExtra = 12; //сколько дополнительно места для окна
    wc.hInstance = hins;
    wc.lpszClassName = "YMK_SnakeName";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);
	hMenu = LoadMenu(hins, MAKEINTRESOURCE(IDR_MENU1));
    wnd = CreateWindow(
        wc.lpszClassName,
        "SNAKE",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        NULL,
		(WIDTH + 3) * pix, // ширина 
        (LENGTH + 5) * pix,// высота
        (HWND)NULL,
        hMenu,
        hins,
        &yd);// указатель на структуру
    ShowWindow(wnd, SW_SHOW);
	UpdateWindow(wnd); 
    while (GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    DestroyWindow(wnd);
	DestroyMenu(hMenu);
    return rc;
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    viewCreateWindows();
    return 0;
}
