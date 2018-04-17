// Agents: misc. agent functions

// Copyright Lyra LLC, 1996. All rights reserved.

#define STRICT

#include <stdio.h>
#include <process.h>
#include "cDDraw.h"
#include "cActorList.h"
#include "cAgentDaemon.h"
#include "cGameServer.h"
#include "Realm.h"
#include "cOutput.h"
#include "cAI.h"
#include "Resource.h"
#include "Main.h"
#include "Options.h"
#include "cLevel.h"
#include "Agent.h"
#include "IconDefs.h"

#ifdef _DEBUG
#define agent_info_file _T("agents-debug.txt")
#else
#define agent_info_file _T("agents.txt")
#endif
#define gamemaster_info_file _T("gamemasters.txt")

//////////////////////////////////////////////////////////////////
// Constants

const int PING_INTERVAL = 1000; // ping main daemon every second

//////////////////////////////////////////////////////////////////
// External Global Variables

extern options_t options;
extern TCHAR gamefile[256];
extern HINSTANCE hInstance;
extern bool framerate;

#ifdef PLAYER_MARE
agent_info_t agent_info[MAX_AGENTS];
HWND hwnd_daemon;
gamemaster_info_t gamemaster_info[MAX_GAMEMASTERS];
bool primary_thread;
cAgentDaemon *daemon;

#else
#ifdef AGENT
extern char agent_gs_ip_address[16];
#endif
#endif

//////////////////////////////////////////////////////////////////
// Agent-Only Globals

cAgentDaemon *daemon;
int tlsLevel;
int tlsPlayer;
int tlsGS;
int tlsActors;
int tlsDD;
int tlsOutput;
int tlsTiming;
int tlsPrimary;
int tlsIndex;
int num_agents=0;
HWND hwnd_daemon = NULL;
agent_info_t agent_info[MAX_AGENTS];
gamemaster_info_t gamemaster_info[MAX_GAMEMASTERS];
bool primary_thread = true, agent_thread = false;

//////////////////////////////////////////////////////////////////
// Functions

// looks up an agents index in the structure; returns -1 if not found
int LookUpAgent(lyra_id_t agent_id)
{
	for (int i=0; i<MAX_AGENTS; i++)
		if (agent_info[i].id == agent_id)
			return i;

	return -1;
}


bool InitAgents(void)
{
	// read agents.txt contents
	int dot = 46; // ASCII code for '.'

	FILE *fh =_tfopen( agent_info_file, _T("r"));
	if (fh == NULL)
	{
		MessageBox(NULL, _T("DOH"), _T("Test"), MB_OK);
		GAME_ERROR(_T("Cannot open agent data"));
		DWORD err = GetLastError();
		return false;
	}

	// initialize address variable
	for (int i=0; i<16; i++)
		agent_gs_ip_address[i]='\0';

	TCHAR buffer[128];
	if ((_ftscanf(fh, _T("%s"), buffer) == EOF)
		|| !(_tcschr(buffer, dot)))
		// can't pull gameserver address and port from agents.txt
	{
		GAME_ERROR(_T("Couldn't load gameserver address information."));
		fclose(fh);
		return false;
	}
	else
	{

#ifdef UNICODE
				wcstombs(agent_gs_ip_address, buffer, _tcslen(buffer)); 
#else
				strcpy(agent_gs_ip_address, buffer); 
#endif

		for (int i=0; i<MAX_AGENTS; i++)
		{
			if ((_ftscanf(fh, _T("%s %d %s %d %d %d %d %d %d"), agent_info[i].name,
				  &(agent_info[i].id),	agent_info[i].passwd, &(agent_info[i].client_port),
				&(agent_info[i].server_port), &(agent_info[i].type),
				&(agent_info[i].level_id), &(agent_info[i].color))) == EOF)
				break;
			agent_info[i].Reset();
			agent_info[i].status = AMsg_AgentInfo::STATUS_READY;
			num_agents++;
		}
	}

	fclose(fh);


	fh =_tfopen( gamemaster_info_file, _T("rt"));
	if (fh == NULL)
	{
		GAME_ERROR(_T("Cannot open gamemaster data"));
		return false;
	}

	for (int i=0; i<MAX_GAMEMASTERS; i++)
	{
		if ((_ftscanf(fh, _T("%s %s"), gamemaster_info[i].name,
			gamemaster_info[i].passwd)) == EOF)
				break;
	}

	fclose(fh);


	// set up thread local storage indexes
	tlsLevel = TlsAlloc();
	tlsPlayer = TlsAlloc();
	tlsGS = TlsAlloc();
	tlsActors = TlsAlloc();
	tlsDD = TlsAlloc();
	tlsOutput = TlsAlloc();
	tlsTiming = TlsAlloc();
	tlsPrimary = TlsAlloc();
	tlsIndex = TlsAlloc();

	// other constraints enforced by agents
	options.network = INTERNET;
	options.exclusive = false;
	framerate = true;

	TlsSetValue(tlsPrimary, &primary_thread);

	return true;
}

void ExitAgents(void)
{	// kill all active threads
	for (int i=0; i<MAX_AGENTS; i++)
		if (agent_info[i].status == AMsg_AgentInfo::STATUS_RUNNING) // threads will quit on WM_QUIT
		{
			StopAgent(i);
			Sleep(500); // give it a msec to log out so we don't overwhelm the game server
		}

	// now de-init everything
	for (int i=0; i<MAX_AGENTS; i++)
		DeInitAgent(i);


	if (daemon) { delete daemon; daemon = NULL; };

	TlsFree(tlsLevel);
	TlsFree(tlsPlayer);
	TlsFree(tlsGS);
	TlsFree(tlsActors);
	TlsFree(tlsDD);
	TlsFree(tlsOutput);
	TlsFree(tlsTiming);
	TlsFree(tlsPrimary);
	TlsFree(tlsIndex);

	hwnd_daemon = NULL;

	return;
}


int StartAgent(lyra_id_t id, int delay)
{
	unsigned long thread_id;

	int index = LookUpAgent(id);

	if (index == -1)
	{
		_tprintf(_T("Trying to start agent - could not look up agent with id %d\n"),id);
		return -1;
	}

	if ((agent_info[index].status != AMsg_AgentInfo::STATUS_READY) &&
		(agent_info[index].status != AMsg_AgentInfo::STATUS_CRASHED))
		return -1;

	agent_info[index].agent_info_index = index;
	agent_info[index].delay = delay;

	thread_id = _beginthread(RunAgent, 0, &(agent_info[index].agent_info_index));

	if (thread_id == -1)
	{
	_tprintf(_T("Could not create new thread for agent with id %d\n"),id);
		return -1;
	}

	return 0;
}


void __cdecl RunAgent( void *param )
{
	MSG	msg;

	srand( timeGetTime() );

	int index = *((int*)param);

	// allocate globals & set up thread local storage

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);

	// important - put this 1st so we don't get immediately killed!
	agent_info[index].last_ping = timeGetTime();

	agent_info[index].status = AMsg_AgentInfo::STATUS_IN_FLUX;

	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(),
		&(agent_info[index].thread_handle), NULL, FALSE, DUPLICATE_SAME_ACCESS);

	TlsSetValue(tlsIndex, &(agent_info[index].agent_info_index));

	TlsSetValue(tlsPrimary, &(agent_thread));

	TCHAR outfile[128];
	_stprintf(outfile, _T("logs/%s"), agent_info[index].name);
	agent_info[index].output_ptr =  new cOutput(outfile, false, true);
	TlsSetValue(tlsOutput, agent_info[index].output_ptr);

	agent_info[index].timing_ptr = new timing_t();
	TlsSetValue(tlsTiming, agent_info[index].timing_ptr);

	agent_info[index].actors_ptr = new cActorList();
	TlsSetValue(tlsActors, agent_info[index].actors_ptr);

	agent_info[index].level_ptr = new cLevel(gamefile);
	TlsSetValue(tlsLevel, agent_info[index].level_ptr);

	agent_info[index].player_ptr =  new cAI(0.0f, 0.0f, 0, agent_info[index].delay, agent_info[index].type);
	TlsSetValue(tlsPlayer, agent_info[index].player_ptr);
	agent_info[index].player_ptr->InitPlayer();

	agent_info[index].cDD_ptr = new cDDraw(NAME, agent_info[index].name, hInstance, AgentWindowProc,
					MAKEINTRESOURCE(IDI_PMARE), IDC_ARROW, 0);
	TlsSetValue(tlsDD, agent_info[index].cDD_ptr);

	agent_info[index].level_ptr->Load(agent_info[index].level_id);

	agent_info[index].gs_ptr = new cGameServer((unsigned short)agent_info[index].client_port, (unsigned short)agent_info[index].server_port);
	TlsSetValue(tlsGS, agent_info[index].gs_ptr);

	agent_info[index].gs_ptr->Login(LOGIN_AGENT);

	agent_info[index].timing_ptr->begin_time = agent_info[index].timing_ptr->t_start = timeGetTime();

	int last_ping = 0;

	agent_info[index].status = AMsg_AgentInfo::STATUS_RUNNING;

	Sleep(500*index); // sleep a bit to avoid pounding gs with logins

	for (;;)
	{
		// process messages until they're gone
		if (PeekMessage(&msg, NULL, 0, 0,PM_REMOVE))
		{
			if ((msg.message == WM_QUIT) || (agent_info[index].cDD_ptr == NULL))
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{	// every 5 seconds, send an "i'm alive" message to daemon
			
			CreateFrame();
			{
//			_tprintf("WARNING: Agent %d threw expection in CreateFrame()::CreateFrame().", agent_info[index].id);
			}
			if ((timeGetTime() - last_ping) > PING_INTERVAL)
			{
				last_ping = timeGetTime();
				PostMessage(hwnd_daemon, WM_PING_DAEMON, timeGetTime(), index);
			}
			Sleep(0);
		}
	}

	DeInitAgent(index);

	_endthread();

	return;
}

// called to stop agents
void StopAgent(int index)
{	
	if (agent_info[index].cDD_ptr && agent_info[index].cDD_ptr->Hwnd_Main())
	{
//		if (agent_info[index].gs_ptr &&
//			agent_info[index].gs_ptr->LoggedIntoGame())
//			agent_info[index].gs_ptr->Logout(GMsg_Logout::LOGOUT_NORMAL);
		PostMessage(agent_info[index].cDD_ptr->Hwnd_Main(), WM_CLOSE, 0, 0);
	}

	return;
}

// called when an agent appears to have hung or crashed
void KillAgent(int index, int status)
{
	if (agent_info[index].status == AMsg_AgentInfo::STATUS_RUNNING)
	{
		HANDLE dead_thread = agent_info[index].thread_handle;
		DeInitAgent(index);
		TerminateThread(dead_thread, -1);
		agent_info[index].status = status;
	}

	return;
}

void DeInitAgent(int index)
{	// clean up thread-specific globals
	if (agent_info[index].status == AMsg_AgentInfo::STATUS_IN_FLUX)
		return; // no recursive calls!

	agent_info[index].status = AMsg_AgentInfo::STATUS_IN_FLUX;

	if (agent_info[index].gs_ptr) { delete agent_info[index].gs_ptr; agent_info[index].gs_ptr= NULL; }
	TlsSetValue(tlsGS, NULL);
	if (agent_info[index].actors_ptr) { delete agent_info[index].actors_ptr; agent_info[index].actors_ptr = NULL; agent_info[index].player_ptr = NULL; }
	if (agent_info[index].level_ptr) { delete agent_info[index].level_ptr; agent_info[index].level_ptr = NULL;}
	if (agent_info[index].cDD_ptr) { agent_info[index].cDD_ptr->DestroyDDraw(); delete agent_info[index].cDD_ptr; agent_info[index].cDD_ptr = NULL; }
	if (agent_info[index].timing_ptr) { delete agent_info[index].timing_ptr; agent_info[index].timing_ptr = NULL; }
	if (agent_info[index].output_ptr) { delete agent_info[index].output_ptr; agent_info[index].output_ptr = NULL; }
	if (agent_info[index].thread_handle) { CloseHandle(agent_info[index].thread_handle); agent_info[index].thread_handle = NULL; }

	agent_info[index].Reset();
	agent_info[index].status = AMsg_AgentInfo::STATUS_READY;

	return;
}
