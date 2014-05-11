// Main: WinMain and Window Proc for Agents

// NOTE - this file is NOT shared with the main project!

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT

#include <winsock2.h>
#include <windows.h>
#include <windowsx.h>
#include "cDDraw.h"
#include "cGameServer.h"
#include "cAgentDaemon.h"
#include "cAgentServer.h"
#include "Mouse.h"
#include "Main.h"
#include "Realm.h"
#include "Constants.h" // from agent service project


/////////////////////////////////////////////////
// Constants

const unsigned int AGENT_CHECKUP_INTERVAL = 30000; // check on agents this often
const unsigned int AGENT_TERMINATE_INTERVAL = 600000; // terminate pingless agents after this interval
const unsigned int MAX_AGENT_CRASHES = 10; // terminate process after this # of thread crashes
const int AGENT_LOGIN_LOGOUT_TIMER = WM_USER + 9235	;

/////////////////////////////////////////////////
// External Global Variables


extern HINSTANCE hInstance; // for main window
extern cGameServer *gs; // game server object
extern cAgentDaemon *daemon; // agent daemon
extern bool exiting; // true when quiting the game
extern LPSTR argv; // command line arguments
extern bool agents_in;

// private shutdown message from agent service
static DWORD wm_shutdown_agents;

/////////////////////////////////////////////////
// Functions

int PASCAL WinMain( HINSTANCE hInst, HINSTANCE hPrevInstance,
                        LPSTR lpCmdLine, int nCmdShow)
   {
   MSG         msg;
   DWORD	   last_checkup;
   int		   num_crashed_agents=0;

   hInstance = hInst;
//   argv = lpCmdLine;
   hPrevInstance = hPrevInstance;

   // bail if there's an initialization problem
   if (!Init_Game()) 
   {
	   Exit();
	   return FALSE;
   }

	wm_shutdown_agents = RegisterWindowMessage(AGENT_SHUTDOWN_STRING);

   // set the daemon to higher priority for better interactivity
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL); 
   
   for (;;)
   {
      // process messages until they're gone
      if (PeekMessage(&msg, NULL, 0, 0,PM_REMOVE))
      {
         if (msg.message == WM_QUIT) return msg.wParam;
		 TranslateMessage(&msg);
		 DispatchMessage(&msg);
      } // main agent thread is just the daemon
	  else
	  {
		if (timeGetTime() - last_checkup > AGENT_CHECKUP_INTERVAL)
		{  // check on agents - terminate hung agents & restart aborted agents
			int now = timeGetTime();
			for (int i=0; i<num_agents; i++)
			{
				if ((agent_info[i].status == AMsg_AgentInfo::STATUS_RUNNING) &&
					((now - agent_info[i].last_ping) > AGENT_TERMINATE_INTERVAL))
				{ // agent must be hung - terminate!
					KillAgent(i, AMsg_AgentInfo::STATUS_CRASHED);
					num_crashed_agents++;
					_tprintf(_T("Agent %d killed at time %d - no ping; num crashes = %d\n"), agent_info[i].id, now, num_crashed_agents);
					if (num_crashed_agents >= MAX_AGENT_CRASHES)
					{ // terminate process & respawn
						printf(_T("Num agent crashes has reached %d; exiting & restarting\n"), num_crashed_agents);
						Exit();
						Sleep(10000); // give everything a few secs to stabilize
						exit(-1);
						return FALSE;
					}
				}
				if ((agent_info[i].status == AMsg_AgentInfo::STATUS_ABORTED) &&
					(now > agent_info[i].restart_time))
				{	// time to restart an aborted agent
					agent_info[i].restart_time = 0;
					agent_info[i].status = AMsg_AgentInfo::STATUS_READY;
					_tprintf(_T("Agent %d restarted at time %d\n"),agent_info[i].id, now);
					StartAgent(agent_info[i].id);
				}
			}
		}

		Sleep(0);
	  }
   }
}


// This window proc is only run by agent threads
LRESULT WINAPI AgentWindowProc ( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{

	if (exiting || (!PrimaryThread() && agent_info[AgentIndex()].status != AMsg_AgentInfo::STATUS_RUNNING)) 
	  // ignore game messages while exiting
		  return DefWindowProc(hWnd, message, wParam, lParam);


	  // treat shutdown command from service as a wm_close
	if (message == wm_shutdown_agents) {
		if (PrimaryThread())
			Exit();
		else
			PostMessage(hwnd_daemon, wm_shutdown_agents, 0, 0);
		return TRUE;
	  }

      switch( message )
      {	
	    case WM_CLOSE:
			if (!PrimaryThread())
			{	// deinit agent thread
				DeInitAgent(AgentIndex());
				PostMessage(hWnd, WM_QUIT, 0, 0);
			}
			break;
#if 0
		case WM_TIMER:
			if (wParam == AGENT_LOGIN_LOGOUT_TIMER)
			{
				for (int i=0; i<num_agents; i++)
				{
					agents_in = false; // we count on being disconnected
					if (agents_in) 
					{
						StopAgent(i);
					} 
					else
					{
						StartAgent(agent_info[i].id, i*5000);
						Sleep(200);
					}
				}
				if (agents_in)
					agents_in = false;
				else
					agents_in = true;
			}
			break;

#endif

		case WM_PING_DAEMON:
			agent_info[(int)lParam].last_ping = (int)wParam;
			break;

		case WM_KEYDOWN: // handle ESC key for main thread 
			if ((wParam == VK_ESCAPE) && (!exiting))
			{
				if (PrimaryThread())
				{  // main thread
					Exit();
					return true;
				}
				else
				{
					PostMessage(hwnd_daemon, WM_CLOSE, 0, 0);
					return true;
				}
			}
			break;

		case WM_GAME_SERVER_DATA:
		  if (gs)
			gs->OnServerUpdate(hWnd, wParam, lParam);
		  return TRUE;

		case WM_DAEMON_LISTEN_DATA:
		  if (daemon)
			daemon->OnListenUpdate(hWnd, wParam, lParam);
		  return TRUE;

		case WM_DAEMON_CLIENT_DATA:
		  if (daemon)
			daemon->OnClientUpdate(hWnd, wParam, lParam);
		  return TRUE;

		case WM_POSITION_UPDATE:
		  if (gs)
		    gs->OnPositionUpdate(hWnd, wParam, lParam);
		  return TRUE; 
     }
   return DefWindowProc(hWnd, message, wParam, lParam);
}
