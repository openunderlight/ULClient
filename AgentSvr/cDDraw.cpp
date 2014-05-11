// Direct Draw Class

// This file is NOT shared with the main project!

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT

#include <winsock2.h>
#include <windows.h>
#include <windowsx.h>
#include "cDDraw.h"

/////////////////////////////////////////////////////////////////
// Class Defintion
 
// Constructor
cDDraw::cDDraw(TCHAR *name, TCHAR *title, HINSTANCE hInstance,
					WNDPROC wproc, LPCTSTR applicon, LPCTSTR applcursor,
					int resolution, int x /*= 0*/,  int y /*= 0*/)
	: res(resolution)
{
	WNDCLASS wc;

	viewx = 480;
	viewy = 300;

	// set up and register window class
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc   = wproc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon( hInstance, applicon);
	wc.hCursor       = LoadCursor( NULL, applcursor );
	wc.hbrBackground = GetStockBrush(BLACK_BRUSH); 
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = name;

	RegisterClass( &wc );

	hwnd_main = CreateWindowEx(
										WS_EX_TOPMOST,
										name,
										title,
										WS_DLGFRAME , 
										0, 0,
										100,
										100,
										NULL,
										NULL,
										hInstance,
										NULL );

	return;
}


void cDDraw::Show() 
{ 
	if (PrimaryThread())
	{	// only show the window for the primary thread
		ShowWindow( hwnd_main, SW_SHOWNORMAL ); 
		UpdateWindow( hwnd_main );
	}
	return;
}

void cDDraw::InitDDraw() {}

unsigned char *cDDraw::GetSurface(int id) { return NULL;}

void cDDraw::ReleaseSurface(int id) {}

int cDDraw::ScaletoRes(int value) { return 0; }

bool cDDraw::EraseSurface( int buffer ) {return true;}

bool cDDraw::BlitOffScreenSurface(void) {return true;}

int cDDraw::DlgPosX(HWND hDlg) { return 0;}

int cDDraw::DlgPosY(HWND hDlg) { return 0;}

bool cDDraw::ShowSplashScreen(void) {return true;};
bool cDDraw::ShowIntroBitmap(void) {return true;};

void cDDraw::ViewRect(RECT *rect) {};


// Selector for window handle

HWND cDDraw::Hwnd_Main(void) { return hwnd_main; }

void cDDraw::DestroyDDraw() {}

// Destructor
cDDraw::~cDDraw() { TlsSetValue(tlsDD, NULL); }

// Invariant checker
#ifdef DEBUG

void cDDraw::Debug_CheckInvariants(int caller) {}

#endif
