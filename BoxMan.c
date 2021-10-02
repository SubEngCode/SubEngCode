#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BoxMan.h"
/* ======================================================================== */
#define BM_Msg(x) printf(x)
/* ======================================================================== */
#define BM_BIT_INDEX_BOX		0
#define BM_BIT_INDEX_MAN		1
#define BM_BIT_INDEX_DOT		2
#define BM_BIT_INDEX_WAL		3
/* ======================================================================== */
#define BM_MASK_BOX			(1 << BM_BIT_INDEX_BOX)
#define BM_MASK_MAN			(1 << BM_BIT_INDEX_MAN)
#define BM_MASK_DOT			(1 << BM_BIT_INDEX_DOT)
#define BM_MASK_WAL			(1 << BM_BIT_INDEX_WAL)
#define BM_SPACE_MASK		0x00
/* ======================================================================== */
#define BM_isMan(b)			(((b) & BM_MASK_MAN)	!= 0)
#define BM_isBox(b)			(((b) & BM_MASK_BOX)	!= 0)
#define BM_isDot(b)			(((b) & BM_MASK_DOT)	!= 0)
#define BM_isWall(b)		(((b) & BM_MASK_WAL) != 0)
#define BM_isSpace(b)		(((b) & 0xFF) == 0)
/* ======================================================================== */
char BM_BlockType[] = "$@.#";
/* ======================================================================== */
/* ======================================================================== */
char BM_Map[]=
	{
	"    #####          \n"
	"    #   #          \n"
	"    #$  #          \n"
	"  ###  $##         \n"
	"  #  $ $ #         \n"
	"### # ## #   ######\n"
	"#   # ## #####  ..#\n"
	"# $  $          ..#\n"
	"##### ### #@##  ..#\n"
	"    #     #########\n"
	"    #######        \n"
	};
/* ======================================================================== */
typedef struct BM__Obj
	{
	UI32 *MapBuf;
	char **map;
	int ColCount, RowCount, dx, dy;
	int man_col, man_row, man_back;
	int box_count, box_col[32], box_row[32], dot_col[32], dot_row[32];
	} BM_Obj;
/* ======================================================================== */
BM_Obj *BM_MainPtr;
/* ======================================================================== */
// LRESULT windowProcedure( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
// HINSTANCE hInstance = GetModuleHandle(NULL);
SI32 BM_Draw(BM_Obj *G, VPTR hwnd);
int BM_New(BM_Obj **PP);
int BM_End(BM_Obj **PP);
int BM_isOk(BM_Obj *G);
int BM_Load(BM_Obj *G, char *FileName);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void AddMenus(HWND);
/* ======================================================================== */
#define IDM_FILE_NEW 1
#define IDM_FILE_OPEN 2
#define IDM_FILE_QUIT 3
/* ======================================================================== */
int BM_main(int argc, char** argv)
	{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	MSG  msg;
	WNDCLASSW wc = {0};

	BM_New(&BM_MainPtr);
	BM_Load(BM_MainPtr, "map/screen.1");

	wc.lpszClassName = L"Simple menu";
	wc.hInstance     = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc   = WndProc;
	wc.hCursor       = LoadCursor(0, IDC_ARROW);

	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName, L"Simple menu",
								WS_OVERLAPPEDWINDOW | WS_VISIBLE,
								10, 10, 600, 400, 0, 0, hInstance, 0);

	while (GetMessage(&msg, NULL, 0, 0))
		{

		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}
	BM_End(&BM_MainPtr);
	return((int) msg.wParam);
	}
//////////////////////////////////////////////////////////////////////////////
int BM_Move(BM_Obj *G, int key, VPTR hwnd)
	{
	int col, row, row2, col2;
	char block;
	char *ManPtrOld, *ManPtrNew;
	char *BoxPtrOld, *BoxPtrNew;
	col = (G->man_col);
	row = (G->man_row);
	ManPtrOld = &(G->map[row][col]);
	if (key == 0x25) // Left
		{
		col--;
		if (col < 0) {return(0);}
		}
	else if (key == 0x27) // Right
		{
		col++;
		if (col >= (G->ColCount)) {return(0);}
		}
	else if (key == 0x26) // Up
		{
		row--;
		if (row < 0) {return(0);}
		}
	else if (key == 0x28) // Down
		{
		row++;
		if (row >= (G->RowCount)) {return(0);}
		}
	ManPtrNew = &(G->map[row][col]);
	block = (*ManPtrNew);
	if (BM_isBox(block))
		{
		col2 = col;
		row2 = row;
		BoxPtrOld = &(G->map[row][col]);
		if (key == 0x25) // Left
			{
			col--;
			if (col < 0) {return(0);}
			}
		else if (key == 0x27) // Right
			{
			col++;
			if (col >= (G->ColCount)) {return(0);}
			}
		else if (key == 0x26) // Up
			{
			row--;
			if (row < 0) {return(0);}
			}
		else if (key == 0x28) // Down
			{
			row++;
			if (row >= (G->RowCount)) {return(0);}
			}
		BoxPtrNew = &(G->map[row][col]);
		block = (*BoxPtrNew);
		if (BM_isSpace(block) || BM_isDot(block))
			{
			(*BoxPtrOld) &= (~BM_MASK_BOX);
			(*BoxPtrNew) |= BM_MASK_BOX;
			(*ManPtrOld) &= (~BM_MASK_MAN);
			(*ManPtrNew) |= BM_MASK_MAN;
			(G->man_col) = col2;
			(G->man_row) = row2;
			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
			return(1);
			}
		}
	else if (BM_isSpace(block) || BM_isDot(block))
		{
		(*ManPtrOld) &= (~BM_MASK_MAN);
		(*ManPtrNew) |= BM_MASK_MAN;
		(G->man_col) = col;
		(G->man_row) = row;
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		return(1);
		}
	return(0);
	}
//////////////////////////////////////////////////////////////////////////////
int BM_End(BM_Obj **PP)
	{
	if ((*PP) != NULL)
		{
		free((*PP));
		(*PP) = NULL;
		}
	return(0);
	}
//////////////////////////////////////////////////////////////////////////////
int BM_New(BM_Obj **PP)
	{
	BM_Obj *G;
	int MemSize, MapSize;
	char *MemPtr, *MapPtr;
	int row, ColCountMax, RowCountMax;
	ColCountMax = 64;
	RowCountMax = 64;
	(*PP) = NULL;
	MapSize = ColCountMax * RowCountMax;
	MapSize = 1024 * ((MapSize + 1023) / 1024);
	MemSize  = sizeof(BM_Obj);
	MemSize += MapSize;
	MemSize += sizeof(VPTR) * RowCountMax;

	MemPtr = (char *) calloc(sizeof(char), MemSize);
	if (MemPtr == NULL)
		{
		return(-__LINE__);
		}
	G = (BM_Obj *) MemPtr;
	MemPtr += sizeof(BM_Obj);

	MapPtr = (char *) MemPtr;
	MemPtr += MapSize;

	(G->map) = (char **) MemPtr;
	MemPtr += sizeof(VPTR) * RowCountMax;
	for (row = 0; row < RowCountMax; row++)
		{
		(G->map[row]) = MapPtr;
		MapPtr += ColCountMax;
		}

	(*PP) = G;
	return(0);
	}
//////////////////////////////////////////////////////////////////////////////
int BM_isOk(BM_Obj *G)
	{
	int row, col, DotIndex;
	char block;
	for (DotIndex = 0; DotIndex < (G->box_count); DotIndex++)
		{
		row = (G->dot_row[DotIndex]);
		col = (G->dot_col[DotIndex]);
		block = (G->map[row][col]);
		if (!BM_isBox(block)) {break;}
		}
	if (DotIndex < (G->box_count)) {return(0);}
	else {return(1);}
	}
//////////////////////////////////////////////////////////////////////////////
int BM_Load(BM_Obj *G, char *FileName)
	{
	int row, col, MemSize, FileSize;
	int ColCount, ColCountNew, RowCount;
	char *map;
	char *DstPtr, *ptr, *ColPtr;
	char *SrcPtr, block;
	int *box_col, *box_row, *dot_col, *dot_row;
	FILE *FilePtr;

	FilePtr = fopen(FileName, "rb");
	fseek(FilePtr, 0, SEEK_END);
	FileSize = ftell(FilePtr);
	MemSize = 1024 * ((FileSize + 1023) / 1024);
	fseek(FilePtr, 0, SEEK_SET);
	map = calloc(sizeof(char), MemSize);
	fread(map, sizeof(char), FileSize, FilePtr);
	fclose(FilePtr);

	SrcPtr = map;
	ColPtr = SrcPtr;
	RowCount = 0;
	ColCount = 0;
	for (col = 0; col < FileSize; col++)
		{
		block = (*SrcPtr++);
		if ((block == '\r') || (block == '\n'))
			{
			RowCount++;
			ColCountNew = SrcPtr - ColPtr - 1;
			if (ColCountNew > ColCount)
				{
				ColCount = ColCountNew;
				}
			if ((*SrcPtr) == '\n') {SrcPtr++;}
			ColPtr = SrcPtr;
			}
		}
	(G->RowCount) = RowCount;
	(G->ColCount) = ColCount;
	box_col = (G->box_col);
	box_row = (G->box_row);
	dot_col = (G->dot_col);
	dot_row = (G->dot_row);
	SrcPtr = map;
	DstPtr = (G->map[0]);
	for (row = 0; row < (G->RowCount); row++)
		{
		(G->map[row]) = DstPtr;
 		for (col = 0; col < (G->ColCount); col++)
			{
			DstPtr[col] = 0;
			}
 		for (col = 0; ; col++)
			{
			block = (*SrcPtr++);
			if (block == '\n')
				{
				break;
				}
			if (block == '\r')
				{
				if ((*SrcPtr) == '\n') {SrcPtr++;}
				break;
				}
			ptr = strchr(BM_BlockType, block);
			if (ptr == NULL) {continue;}
			block = ptr - BM_BlockType;
			block = (1 << block);
			DstPtr[col] = block;
			if (BM_isBox(block))
				{
				(*box_col++) = col;
				(*box_row++) = row;
				}
			if (BM_isMan(block))
				{
				(G->man_col) = col;
				(G->man_row) = row;
				}
			if (BM_isDot(block))
				{
				(*dot_col++) = col;
				(*dot_row++) = row;
				}
			}
		DstPtr += (G->ColCount);
		if ((*SrcPtr) == 0) {break;}
		}
	(G->box_count)  = box_col - (G->box_col);
	(G->man_back)		= ' ';
	free(map);
	return(0);
	}
//////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
												 WPARAM wParam, LPARAM lParam)
	{
	switch(msg)
		{
    case WM_KEYDOWN:
    	BM_Move(BM_MainPtr, (int) wParam, hwnd);
//			wprintf_s(L"WM_KEYDOWN: 0x%x\n", wParam);
        //OutputDebugString(msg);
        break;
    case WM_PAINT:   // drawing the content of window.
			{
			BM_Draw(BM_MainPtr, hwnd);
			break;
			}

		case WM_CREATE:

			AddMenus(hwnd);
			break;

		case WM_COMMAND:

			switch(LOWORD(wParam))
				{

				case IDM_FILE_NEW:
				case IDM_FILE_OPEN:

					MessageBeep(MB_ICONINFORMATION);
					break;

				case IDM_FILE_QUIT:

					SendMessage(hwnd, WM_CLOSE, 0, 0);
					break;
				}

			break;

		case WM_DESTROY:

			PostQuitMessage(0);
			break;
		}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
	}

void AddMenus(HWND hwnd)
	{
	HMENU hMenubar;
	HMENU hMenu;

	hMenubar = CreateMenu();
	hMenu = CreateMenu();
	AppendMenuW(hMenu, MF_STRING, IDM_FILE_NEW, L"&New");
	AppendMenuW(hMenu, MF_STRING, IDM_FILE_OPEN, L"&Open");
	AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(hMenu, MF_STRING, IDM_FILE_QUIT, L"&Quit");
	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR) hMenu, L"&File");
	SetMenu(hwnd, hMenubar);
	}

SI32 BM_Draw(BM_Obj *G, VPTR hwnd)
	{
	char *BlockPtr;
	PAINTSTRUCT ps;
	HDC dc;
	RECT r;
	RECT R2;
	SI32 dx, dy, x, y;
	SI32 Row, Col, RowCount, ColCount;
	int Gray, block;

	HBRUSH ColorList[16];
	HBRUSH FillColor, SideColor;

	Gray = 200;

	ColorList[BM_BIT_INDEX_WAL] = CreateSolidBrush(RGB(  0,   0,   0)); // Wall
	ColorList[BM_BIT_INDEX_BOX] = CreateSolidBrush(RGB(255,   0,   0)); // Box
	ColorList[BM_BIT_INDEX_DOT] = CreateSolidBrush(RGB(  0,   0, 255)); // Dot
	ColorList[BM_BIT_INDEX_MAN] = CreateSolidBrush(RGB(255,   0, 255)); // Man
	if (BM_isOk(G))
		{
		ColorList[4] = CreateSolidBrush(RGB(128, 255, 128)); // Space
		}
	else
		{
		ColorList[4] = CreateSolidBrush(RGB(255, 255, 255)); // Space
		}
	ColorList[BM_BIT_INDEX_WAL + 5] = ColorList[BM_BIT_INDEX_WAL];
	ColorList[BM_BIT_INDEX_BOX + 5] = CreateSolidBrush(RGB(Gray, Gray, Gray));
	ColorList[BM_BIT_INDEX_DOT + 5] = CreateSolidBrush(RGB(Gray, Gray, Gray));
	ColorList[BM_BIT_INDEX_MAN + 5] = CreateSolidBrush(RGB(Gray, Gray, Gray));
	ColorList[9] = ColorList[4];

	dx = 30;
	dy = 30;
	RowCount = (G->RowCount);
	ColCount = (G->ColCount);

	GetClientRect(hwnd,&r);
	dc=BeginPaint(hwnd,&ps);
//
//	HPEN hPen = GetStockObject(BLACK_PEN);
//	HPEN hOldPen = SelectObject(dc, hPen);

//	x = 20;
//	y = 20;
//	for (k = 0; k <= 32; k++)
//		{
//		MoveToEx(dc, x, y, NULL);
//		LineTo(dc, x + w, y);
//		y += dy;
//		}
//
	x = 0;
	y = 0;
	BlockPtr = (G->map[0]);
	for (Row = 0; Row < RowCount; Row++)
		{
		x = 0;
		for (Col = 0; Col < ColCount; Col++)
			{
			block = (int) (*BlockPtr++);
//			if (BM_isSpace(block)) {block = 0;}
			if			(BM_isMan(block)) 	{block = BM_BIT_INDEX_MAN;}
			else if (BM_isBox(block)) 	{block = BM_BIT_INDEX_BOX;}
			else if (BM_isWall(block))	{block = BM_BIT_INDEX_WAL;}
			else if (BM_isDot(block)) 	{block = BM_BIT_INDEX_DOT;}
			else {block = 4;}
			FillColor = ColorList[block];
			SideColor = ColorList[block + 5];
			SetRect(&R2, x, y, x + dx, y + dy);
			FillRect(dc,  &R2, FillColor);
			FrameRect(dc, &R2, SideColor);
			x = x + dx;
			}
		y = y + dy;
		}
	EndPaint(hwnd,&ps);
	return(0);
	}
