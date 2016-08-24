// Header file for realm.cpp

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef REALM_H
#define REALM_H

#include <windows.h>
#include "Central.h"

//////////////////////////////////////////////////////////////////
// New Window Messages

//////////////////////////////////////////////////////////////////
// Constants

#ifdef PMARE
const unsigned int START_LEVEL_ID = 44;
#else
const unsigned int START_LEVEL_ID = 1;
#endif
const unsigned int RECRUITING_LEVEL_ID = 20;
const int XOFF = 0;
const int YOFF = 0;
const TCHAR FONT_NAME[16] = _T("Arial");
// consts that go with windows color changes
const unsigned int ORANGE = 0x003399FF;
const unsigned int BLACK =  0x00000000;
const unsigned int BLUE = 0x00996633;
const unsigned int LTBLUE = 0x00CC9966;
const unsigned int DKBLUE = 0x00663300;
const unsigned long lyra_colors[11] =
	{BLUE, LTBLUE, DKBLUE, LTBLUE, ORANGE, BLUE, 
	 ORANGE, BLACK, ORANGE, ORANGE, BLUE};

const int syscolors[11] = 
	{COLOR_BTNFACE, COLOR_BTNHIGHLIGHT, 
	 COLOR_BTNSHADOW, COLOR_3DLIGHT, COLOR_BTNTEXT, COLOR_HIGHLIGHT,
	 COLOR_HIGHLIGHTTEXT, COLOR_WINDOW, COLOR_WINDOWTEXT, COLOR_GRAYTEXT,
	 COLOR_INFOBK};

extern int tlsTiming;

#define NAME _T("Underlight")
#define TITLE _T("Underlight")
#define REGISTRY_DATA_KEY _T("Software\\Lyra\\Underlight\\1.0")
#define AGENT_REGISTRY_KEY _T("Software\\Lyra\\AgentController\\1.0")

struct timing_t
{
	float	nticks;		// number of ticks since last update
	long	nmsecs;		// msecs since last update
	long	sync_ticks;	// ticks synched across the network 
	int	sync_msecs;	// msecs synched
	// internal to Utils (used to calculate the above)
	int	t_start;
	int	t_end;
	// profiling variables
	int	begin_time; // timing value
	int	frames;		// # of frames displayed
	// interal to CreateFrame
	int	lastFrame;
	int	lastDisplayTime;
	int	lastPositionUpdate;
	int	lastServerUpdate;
	int	lastPacketCheck;
	int	lastPacketCount;
	int	lastDragScrollCheck;
	int	frameCount;
	int	lastInvCountUpdate;

	inline timing_t() { memset(this,0,sizeof(timing_t));}

#ifdef AGENT
	~timing_t(void) { TlsSetValue(tlsTiming, NULL); }
#endif

};

struct pmare_t {
	UINT	charge;
	UINT	name; // pointer to string table
	UINT	descrip; // pointer to string table
};

struct ppoint_t {
	int		cursel; // current selection for use type (permanent stat, etc.)
	int		sub_cursel; // current subselection (which stat, which art, etc.)
	int		cost;
	int		art_id;
	int		skill;
	bool    in_use;

	inline void reset(void) { 
		cursel = -1; 
		sub_cursel = -1; 
		cost = -1; 
		art_id = -1; 
		skill = -1; 
		in_use = false;
 
	};
};

//////////////////////////////////////////////////////////////////
// Function Prototypes

void __cdecl CreateFrame(void);
bool __cdecl Init_Game(void);
void __cdecl Exit(void);
void __cdecl StartExit(void);
void __cdecl CancelExit(void);
void __cdecl EstimatePmareBilling(void);
#define MemoryCheck(label)

#endif
