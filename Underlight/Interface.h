// Header file for Interface.cpp

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef INCL_INTERFACE
#define INCL_INTERFACE

#include "Central.h"

struct window_pos_t;

//////////////////////////////////////////////////////////////////
// Constants

//////////////////////////////////////////////////////////////////
// Function Prototypes for the Lyra Dialog Controller

void __cdecl InitLyraDialogController(void);
void __cdecl DeInitLyraDialogController(void);
HWND __cdecl CreateLyraDialog( HINSTANCE hInstance, int dialog, HWND hWndParent, 
											 DLGPROC lpDialogFunc);
int __cdecl LyraDialogBox( HINSTANCE hInstance, int dialog, HWND hWndParent, 
											 DLGPROC lpDialogFunc);

bool IsLyraDialogMessage(MSG *msg); // used in main message loop



enum blit_types {
	STRETCH, 
	NOSTRETCH,
}; 

// utilities
// void BlitBitmap(HDC dc, HBITMAP bitmap, RECT *region, const window_pos_t& rcregion, int mask = SRCCOPY);
void BlitBitmap(HDC dc, HBITMAP bitmap, RECT *region, int stretch, int mask = SRCCOPY);
void TransparentBlitBitmap(HDC dc, int bitmap_id, RECT *region, 
						   int stretch, int mask = SRCCOPY);
void DrawListBoxArrow(HWND hWnd, HDC dc, RECT r);
bool IsSpecialListBox(HWND hWnd);

HCURSOR __cdecl BitmapToCursor(HBITMAP bmp, HCURSOR cursor);

HBITMAP CreateWindowsBitmap(int bitmap_id);
void CreateWindowsBitmaps(int bitmap_id, HBITMAP hBitmaps[]);

HBITMAP *CreateControlBitmaps( int effect_id, int num_states=1);
void DeleteControlBitmaps(HBITMAP *bitmaps);

void ResizeButton(HWND hWnd, int new_width, int new_height);
void ResizeLabel(HWND hWnd, int new_width, int new_height);
void ResizeDlg(HWND hDlg);

bool TileBackground(HWND hWnd, int which_bitmap=0);

HBRUSH SetControlColors(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam, bool goalposting=false);

void DisableTalkDialogOptionsForInvisAvatar(HWND hWindow);

// MFC helpers
void Draw3DRect(HDC dc, RECT* lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight);
void Draw3DRect(HDC dc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);
void DeflateRect(RECT *rect, int x, int y);
void FillSolidRect(HDC dc, int x, int y, int cx, int cy, COLORREF clr);

// overloaded window procs
BOOL CALLBACK LyraDialogProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PushButtonProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK StateButtonProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK StaticTextProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ListBoxProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ComboBoxProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TrackBarProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK EditProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK GenericProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK EnumChildProcSetup( HWND hChild, LPARAM lParam);
BOOL CALLBACK EnumChildProcSize( HWND hChild, LPARAM lParam);

#endif