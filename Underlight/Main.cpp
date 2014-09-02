// Main: WinMain and Window Proc

// Copyright Lyra LLC, 1996. All rights reserved.

#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include "cDDraw.h"
#include "cGameServer.h"
#include "cAgentServer.h"
#include "cPlayer.h"
#include "cKeymap.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Main.h"
#include "Realm.h"
#include "Utils.h"
#include "Dialogs.h"
#include "cPostGoal.h"
#include "cPostQuest.h"
#include "cReportGoal.h"
#include "Interface.h"
//#include "RogerWilco.h"

/////////////////////////////////////////////////
// Global Variables


/////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;	// for main window
extern cGameServer *gs;			// game server object
extern cAgentServer *as;		// agent server object
extern cPlayer *player;
extern cDDraw *cDD;
extern cPostGoal *postgoal;
extern cPostQuest *postquest;
extern cReportGoal *reportgoal;
extern bool exiting;				// true when quiting the game
extern LPTSTR argv;				// command line arguments
extern int argc;
extern DWORD last_keystroke;

const int MIN_FRAME_TIMER = WM_USER + 9234;
 
/////////////////////////////////////////////////
// Functions


int PASCAL WinMain( HINSTANCE hInst, HINSTANCE hPrevInstance,
								LPSTR lpCmdLine, int nCmdShow)
	{
#ifdef UL_DEBUG
//	DebugBreak();
#endif

	MSG msg;

	hInstance = hInst;
#ifdef UNICODE
//	argv = CommandLineToArgvW(GetCommandLineW(), &argc);
#else
	argv = lpCmdLine; // ASCII only
#endif
	hPrevInstance = hPrevInstance;

	// bail if there's an initialization problem
	if (!Init_Game())
	{
		Exit();
		return FALSE;
	}

	last_keystroke = LyraTime();

	for (;;)
	{
		// process messages until they're gone
		if (PeekMessage(&msg, NULL, 0, 0,PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				return msg.wParam;
			}

			switch (msg.message)
			{	// avoid idle timeouts
				case WM_KEYUP:
				case WM_KEYDOWN:
				case WM_CHAR:
				case WM_LBUTTONDOWN:
				case WM_RBUTTONDOWN:
				case WM_MOUSEMOVE:
					last_keystroke = LyraTime();
					break;
			}

			if (!IsLyraDialogMessage(&msg) &&
				!(reportgoal->Active() && IsDialogMessage(reportgoal->Hwnd(),&msg)) &&
				!(postgoal->Active() && IsDialogMessage(postgoal->Hwnd(),&msg)) &&
				!(postquest->Active() && IsDialogMessage(postquest->Hwnd(),&msg)))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else // no messages, or frame timer 
			CreateFrame();
	}
}

LRESULT WINAPI WindowProc ( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static HCURSOR hCursor = LoadCursor(NULL,IDC_ARROW); // for cursor reset

	if (exiting) // ignore game messages while exiting
		return DefWindowProc(hWnd, message, wParam, lParam);

//	if (message == (RWNET_MESSAGE)) // message constant, channel id, member id
//	{
//		HandleRWMessage(LOWORD(wParam), (RWNET_CHANNELDESC*)HIWORD(wParam), (RWNET_MEMBERDESC*)lParam);
//		return TRUE;
//	}

	switch( message )
	{
		HANDLE_MSG(hWnd, WM_KEYDOWN, Realm_OnKey);
		HANDLE_MSG(hWnd, WM_KEYUP, Realm_OnKey);
		HANDLE_MSG(hWnd, WM_SYSKEYDOWN, Realm_OnKey);
		HANDLE_MSG(hWnd, WM_SYSKEYUP, Realm_OnKey);
		HANDLE_MSG(hWnd, WM_CHAR, Realm_OnChar);
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, Realm_OnLButtonDown);
		HANDLE_MSG(hWnd, WM_MBUTTONDOWN, Realm_OnMButtonDown);
		HANDLE_MSG(hWnd, WM_RBUTTONDOWN, Realm_OnRButtonDown);
		HANDLE_MSG(hWnd, WM_KILLFOCUS, Realm_OnKillFocus);
		HANDLE_MSG(hWnd, WM_MOUSEMOVE, Realm_OnMouseMove);
		HANDLE_MSG(hWnd, WM_LBUTTONUP, Realm_OnLButtonUp);
		HANDLE_MSG(hWnd, WM_MBUTTONUP, Realm_OnMButtonUp);
		HANDLE_MSG(hWnd, WM_RBUTTONUP, Realm_OnRButtonUp);
		//HANDLE_MSG(hWnd, WM_COMMAND, Realm_OnCommand);

	case WM_MOUSEWHEEL:
		Realm_OnMouseWheelScroll(hWnd, LOWORD(lParam), HIWORD(lParam), (short)HIWORD(wParam));

	case WM_TIMER:
		if (wParam == MIN_FRAME_TIMER)
			CreateFrame();
		break;


	case WM_CLOSE:
	  if (!exiting)
      StartExit ();
		break;

	case WM_SETFOCUS:
		if (cDD && (hWnd == cDD->Hwnd_Main()))
			SetCursor(hCursor);
		break;

	case WM_GAME_SERVER_DATA:
		if (gs)
			gs->OnServerUpdate(hWnd, wParam, lParam);
		return TRUE;

	case WM_POSITION_UPDATE:
		if (gs)
			gs->OnPositionUpdate(hWnd, wParam, lParam);
		return TRUE;

#ifdef GAMEMASTER
	case WM_GM_QUERY:		// respond to "Are you a GM?"
		if (memcmp("?GM?", &lParam, 4)==0)
			return Lyra::GAME_VERSION + Lyra::GM_DELTA;
#endif

#ifdef GAMEMASTER
	case WM_AGENT_SERVER_DATA:
		as->OnServerUpdate(hWnd, wParam, lParam);
		return TRUE;
#endif

	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

