// Header file for Agent.cpp

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef AGENT_H
#define AGENT_H

#include "LyraDefs.h"
#include <windows.h>
#include <process.h>

/////////////////////////////////////////////////
// Constants

#define WM_PING_DAEMON WM_USER + AGENTS_MAGIC + 1



/////////////////////////////////////////////////
// Declarations

class cLevel;
class cPlayer;
class cGameServer;
class cActorList;
class cDDraw;
class cOutput;
class cAgentDaemon;
struct timing_t;


struct agent_info_t { // everything you need to know about an agent...
	lyra_id_t	id; // player id for monster
	int			agent_info_index; // index in agent_info array
	TCHAR		name[Lyra::PLAYERNAME_MAX];
	TCHAR		passwd[Lyra::PLAYERNAME_MAX];
	int			client_port;
	int			server_port;
	int			type;
	int			status;
	int			last_ping;
	int			framerate;
	int			level_id;
	int			restart_time;
	int			delay;			// delay before starting agent
	int			color; // coloration for avatar; 0, 1, or 2
	cLevel		*level_ptr;
	cPlayer		*player_ptr;
	cGameServer *gs_ptr;
	cActorList  *actors_ptr;
	cDDraw		*cDD_ptr;	
	cOutput		*output_ptr;
	timing_t	*timing_ptr;
	HANDLE	    thread_handle;

	inline agent_info_t() { memset(this,0,sizeof(agent_info_t)); }
	inline void agent_info_t::Reset() {
		framerate = last_ping = status = 0;
		level_ptr= NULL; player_ptr= NULL;
		gs_ptr= NULL; actors_ptr= NULL;
		cDD_ptr= NULL; output_ptr= NULL;
		timing_ptr = NULL; thread_handle = NULL;
	}
};

struct gamemaster_info_t {
	TCHAR		name[Lyra::PLAYERNAME_MAX];
	TCHAR		passwd[Lyra::PLAYERNAME_MAX];
	bool		logged_in;
	int			socket_index; // index # of this user's socket

	inline gamemaster_info_t() 
	{ 
		memset(this,0,sizeof(gamemaster_info_t)); 
		logged_in = false; socket_index = -1;
	}
};


#ifdef AGENT
// We need to include IsAgent before Central, since some
// defines in Central.h need to know what build we're running
#include "IsAgent.h"
#endif

#include "Central.h"

#endif

