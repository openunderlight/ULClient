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
#include <string>
#include <memory>
#include "Mouse/MouseClass.h"

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
extern bool IsLyraColors;
extern bool show_splash;
const int MIN_FRAME_TIMER = WM_USER + 9234;
extern unsigned int show_splash_end_time;
//initialise base mouseclass
static MouseClass mouse;

// function for finding mouse:)
bool WindowContainer() 
{
	//generic bool for if mouse is found
	static bool raw_input_initialized = false;
	//if it hasn't been found yet.
	if (!raw_input_initialized)
	{
		// make default raw input device
		RAWINPUTDEVICE rid;

		//assign basic info to device info
		rid.usUsagePage = 0x01; //typical usage group for input
		rid.usUsage = 0x02; // mouse
		rid.dwFlags = 0; //default
		rid.hwndTarget = NULL; // default

		//register device with windows, see if it finds it.
		if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
		{
			DebugOut("Failed to register raw input devices");
			exit(-1);
			//registration failled. call GetLastError for cause of error
		}
		else
		{
			//otherwise we found it! yay
			raw_input_initialized = true;
			DebugOut("Mouse should be initiatilzed");
		}
		//return the mouse
		return raw_input_initialized;
	}
}

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
	show_splash = true;
	show_splash_end_time = LyraTime() + 4600;

	//go get the mouse ready
	WindowContainer();

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
				case WM_INPUT: //rawmouse mesg worker
				{
					//check all msgs
					while (!mouse.EventBufferIsEmpty())
					{
						//make a mosue event from mouseclass raw input
						MouseEvent me = mouse.ReadEvent();
						if (me.GetType() == MouseEvent::EventType::RAW_MOVE) //if msg is actually a raw movement
						{
							//set our helper to this mouse event for use later
							MeHelper::SetME(me);
							// rest of this is to spam the debug chat window in visualstudio the relative cords.
							std::string outmsg = "x: " + std::to_string(me.GetPosX());
							outmsg += ", Y: " + std::to_string(me.GetPosY());
							outmsg += "\n";
							OutputDebugStringA(outmsg.c_str());
							
						}
					}
				break;
				}
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
		Realm_OnMouseWheelScroll(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (short)HIWORD(wParam));

	case WM_TIMER:
		if (wParam == MIN_FRAME_TIMER)
			CreateFrame();
		break;


	case WM_INPUT:
	{
		UINT dataSize = 0;

		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

		if (dataSize > 0)
		{
			std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
				if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
				{
					RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
					if (raw->header.dwType == RIM_TYPEMOUSE)
					{
						mouse.OnMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
					}
				}
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
#ifndef AGENT
	case WM_QUERYOPEN:
		if (!IsLyraColors)
		{
			SetSysColors(11, syscolors, lyra_colors);
			IsLyraColors = TRUE;
		}
		break;
#endif

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

