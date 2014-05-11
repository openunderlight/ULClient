// Header file for cAgentBox class 

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef CAGENTBOX_H
#define CAGENTBOX_H

#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include "AMsg_All.h"
#include "LyraDefs.h"

//////////////////////////////////////////////////////////////////
// Constants

const int NO_AGENT = -1;

//////////////////////////////////////////////////////////////////
// Enumerations

//////////////////////////////////////////////////////////////////
// New Windows Messages

//////////////////////////////////////////////////////////////////
// Helpers

LRESULT WINAPI AgentBoxWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int CALLBACK CompareAgents(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); 

//////////////////////////////////////////////////////////////////
// Class Definition
class cAgentBox
{

public: 

private:
	HWND hwnd_agent_box; 
	HWND hwnd_listview; 
	HWND hwnd_start;
	HWND hwnd_start_all;
	HWND hwnd_stop;  
	HWND hwnd_stop_all;  
	HWND hwnd_goto;
	HWND hwnd_posess;
	HWND hwnd_hide; 
	HWND hwnd_close;
#ifdef UL_DEBUG
	HWND hwnd_kill; 
#endif

	int num_agents;

	// for possession
	TCHAR old_username[Lyra::PLAYERNAME_MAX];
    TCHAR old_password[Lyra::PASSWORD_MAX];
	float  old_x, old_y;
	int old_level_id;
	bool posession_pending;
	bool posession_in_progress;
	bool exorcism_in_progress;
	bool ready_to_posess;
	lyra_id_t	possessed_id;
	int posess_x, posess_y, posess_level_id;
	int posess_time;
	SOCKADDR_IN  game_server_addr; // IP address/port of orig game server
	

public:
    cAgentBox(void);
    ~cAgentBox(void);

	inline void SetServerAddress(SOCKADDR_IN value) { game_server_addr = value; }
	inline SOCKADDR_IN ServerAddress(void) { return game_server_addr; }

	void Show(void);
	void Hide(void);

	int LookUpAgentIndex(lyra_id_t id);
	lyra_id_t LookUpAgent(int index);

	void AddAgent(lyra_id_t id, const TCHAR *name, lyra_id_t level_id, int type);
	void RemoveAgent(lyra_id_t id);
	void RemoveAllAgents(void);
	void UpdateAgent(AMsg_AgentInfo& info);
	int  SelectedIndex(void);
	void SortAgents(void);
	void Update(void); 
	void Posess(AMsg_PosessInfo& info);
	void SetButtonState(void);
	bool const PosessionInProgress(void) const {return posession_in_progress;}
	bool const ExorcismInProgress(void) const {return exorcism_in_progress;}
	void SetExorcismInProgress(bool value)  {exorcism_in_progress = value;}

private:

	void SetSubItem(int index, int subitem, int data);

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cAgentBox(const cAgentBox& x);
	cAgentBox& operator=(const cAgentBox& x);

	// The Window Proc & compare procs for this control must be friends
	friend LRESULT WINAPI AgentBoxWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	friend int CALLBACK CompareAgents(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); 

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

};

#endif