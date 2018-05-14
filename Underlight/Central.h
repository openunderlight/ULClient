// Central Include File - Included by All

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef CENTRAL_H
#define CENTRAL_H
#include <string.h>
#include <limits.h>
#include <winsock2.h>
#ifndef STRICT
#define STRICT
#endif

#include "agent.h"

class cOutput;

//////////////////////////////////////////////////////////////////
// Constants


#define CHECK_INVARIANTS

#define DEAD_SECTOR 0 // sector for nondisplayed actors

const int DEFAULT_MESSAGE_SIZE = 1024;
const int DEAD_X = _I16_MAX -1;
const int DEAD_Y = _I16_MAX -1;
const int DEAD_Z = _I16_MAX -1;

//////////////////////////////////////////////////////////////////
// Convenience Variables
//
// Most of these are globals to make it easy to use the stringtable

extern TCHAR message[DEFAULT_MESSAGE_SIZE];
extern TCHAR disp_message[DEFAULT_MESSAGE_SIZE];
extern TCHAR temp_message[DEFAULT_MESSAGE_SIZE];
extern TCHAR duration_message[DEFAULT_MESSAGE_SIZE];
extern TCHAR modifier_message[DEFAULT_MESSAGE_SIZE];
extern TCHAR guild_name_message[DEFAULT_MESSAGE_SIZE];
extern TCHAR guild_rank_message[DEFAULT_MESSAGE_SIZE];
extern TCHAR guild_belief_combo_message[DEFAULT_MESSAGE_SIZE];
extern TCHAR guild_goal_message[DEFAULT_MESSAGE_SIZE];
extern TCHAR color_message[DEFAULT_MESSAGE_SIZE];
extern TCHAR monster_color_message[DEFAULT_MESSAGE_SIZE];
extern TCHAR token_message[DEFAULT_MESSAGE_SIZE];
extern TCHAR nightmare_message[DEFAULT_MESSAGE_SIZE];
extern TCHAR dreamweapon_message[DEFAULT_MESSAGE_SIZE];
extern TCHAR talisman_message[DEFAULT_MESSAGE_SIZE];
extern TCHAR values_select[100][DEFAULT_MESSAGE_SIZE];

extern TCHAR errbuf[DEFAULT_MESSAGE_SIZE];


extern cOutput *output;

//////////////////////////////////////////////////////////////////
// Macros

// Types of errors.

enum error_typnete {
  GENERIC_ERR = 0,            // invariant broken or other app-defined error
  WINDOWS_ERR = 1,           // windows error
  SOCKETS_ERR = 2,		  // sockets error
  ASSERT_ERR = 3,           // failed assertion
};

#ifdef UL_DEBUG
#define ASSERT(exp) { if (!(exp)) { ErrAndExit(ASSERT_ERR, IDS_ASSERT_FAILED, __LINE__, _T(__FILE__)); } }
#define VERIFY(exp) { if (!(exp)) { ErrAndExit(ASSERT_ERR, IDS_ASSERT_FAILED, __LINE__, _T(__FILE__)); } }
// since X and Y are often stored in shorts, max x/y is radix of a short.
// do not use an inline, as that would not allow type to breal and thus not display problem
#define ASSERT_XY(X,Y) { if ( (X<_I16_MIN) || (X>_I16_MAX) || (Y<_I16_MIN) || (Y>_I16_MAX) )\
{ ErrAndExit(ASSERT_ERR, IDS_XY_LIMIT, __LINE__, _T(__FILE__)); }}
#define VERIFY_XY(X,Y) { if ( (X<_I16_MIN) || (X>_I16_MAX) || (Y<_I16_MIN) || (Y>_I16_MAX) )\
{ ErrAndExit(ASSERT_ERR, IDS_XY_LIMIT, __LINE__, _T(__FILE__)); }}

#else
#define ASSERT(x)
#define ASSERT_XY(x)
#define VERIFY(exp) { if (!(exp)) { Warn(IDS_ASSERT_FAILED, __LINE__, _T(__FILE__)); } }
// since X and Y are often stored in shorts, max x/y is radix of a short.
// do not use an inline, as that would not allow type to breal and thus not display problem
#define VERIFY_XY(X,Y) { if ( (X<=_I16_MIN) || (X>=_I16_MAX) || (Y<=_I16_MIN) || (Y>=_I16_MAX)  )\
{ Warn(IDS_XY_LIMIT, __LINE__, _T(__FILE__)); }}

#endif

// 4 error levels: warn, non-fatal, and game error
// info writes a warning to the log in debug mode
// warn writes a warning to the log & submits a bug report
// game error stops the game & displays an exit message
// non-fatal will be a warn in release mode & a game error in debug mode
// show _LINE/FILE to debug and gamemaster only
#if defined(UL_DEBUG) || defined (GAMEMASTER)
#define INFO(msg) { Info(msg, __LINE__, _T(__FILE__)); }
#define WARN(msg) { Warn( msg, __LINE__, _T(__FILE__)); }
#define GAME_ERROR(msg) { ErrAndExit(GENERIC_ERR, msg, __LINE__, _T(__FILE__)); }
#define GAME_ERROR1(msg, p1) { ErrAndExit2(GENERIC_ERR, msg, __LINE__, _T(__FILE__), 0, p1); }
#define GAME_ERROR2(msg, p1, p2) { ErrAndExit2(GENERIC_ERR, msg, __LINE__, _T(__FILE__), 0, p1, p2); }
#define GAME_ERROR3(msg, p1, p2, p3) { ErrAndExit2(GENERIC_ERR, msg, __LINE__, _T(__FILE__), 0, p1, p2, p3); }
#define GAME_ERROR4(msg, p1, p2, p3, p4) { ErrAndExit2(GENERIC_ERR, msg, __LINE__, _T(__FILE__), 0, p1, p2, p3, p4); }
#define GAME_ERROR5(msg, p1, p2, p3, p4, p5) { ErrAndExit2(GENERIC_ERR, msg, __LINE__, _T(__FILE__), 0, p1, p2, p3, p4, p5); }
#define SOCKETS_ERROR(err) { ErrAndExit(SOCKETS_ERR, (TCHAR*)NULL, __LINE__, _T(__FILE__), err); }
#define WINDOWS_ERROR() { ErrAndExit(WINDOWS_ERR, (TCHAR*)NULL, __LINE__, _T(__FILE__)); }
#else
#define INFO(msg) { Info(msg); }
#define WARN(msg) { Warn(msg); }
#define GAME_ERROR(msg) { ErrAndExit(GENERIC_ERR, msg); }
#define GAME_ERROR1(msg, p1) { ErrAndExit2(GENERIC_ERR, msg, __LINE__, _T(__FILE__), 0, p1); }
#define GAME_ERROR2(msg, p1, p2) { ErrAndExit2(GENERIC_ERR, msg, __LINE__, _T(__FILE__), 0, p1, p2); }
#define GAME_ERROR3(msg, p1, p2, p3) { ErrAndExit2(GENERIC_ERR, msg, __LINE__, _T(__FILE__), 0, p1, p2, p3); }
#define GAME_ERROR4(msg, p1, p2, p3, p4) { ErrAndExit2(GENERIC_ERR, msg, __LINE__, _T(__FILE__), 0, p1, p2, p3, p4); }
#define GAME_ERROR5(msg, p1, p2, p3, p4, p5) { ErrAndExit2(GENERIC_ERR, msg, __LINE__, _T(__FILE__), 0, p1, p2, p3, p4, p5); }
#define SOCKETS_ERROR(err) { ErrAndExit(SOCKETS_ERR, (TCHAR*)NULL, 0, _T(""), err); }
#define WINDOWS_ERROR() { ErrAndExit(WINDOWS_ERR, (TCHAR*)NULL); }
#endif


//#ifdef _DEBUG // non-fatal errors cause crash
//#define NONFATAL_ERROR GAME_ERROR
//#else
#define NONFATAL_ERROR WARN
//#endif

//////////////////////////////////////////////////////////////////
// Magic Numbers

// All error codes, window messages, and other numbers used within
// each module is offset by the magic number to avoid duplication.


#define REALM_MAGIC			100
#define LG_MAGIC			200 
#define GS_MAGIC			300
#define RS_MAGIC			400
#define DIALOG_MAGIC		500
#define PARTY_MAGIC			600
#define PLAYER_MAGIC		700
#define DSOUND_MAGIC		800
#define CHAT_MAGIC			900
#define CONTROL_PANEL_MAGIC 1000
#define WAVE_MAGIC			1100
#define ACTOR_MAGIC			1200
#define ACTORLIST_MAGIC		1300
#define DDRAW_MAGIC			1400
#define MISSILE_MAGIC		1500
#define NEIGHBOR_MAGIC		1600
#define INIT_MAGIC			1700
#define RENDER_MAGIC		1800
#define MOVE_MAGIC			1900
#define SHADER_MAGIC		2000
#define BANNER_MAGIC        2100
#define CENTRAL_MAGIC		2200
#define UTILS_MAGIC			2300
#define ITEMBITMAPS_MAGIC   2400
#define ITEM_MAGIC			2500
#define SENDING_MAGIC       2600
#define EFFECT_MAGIC	    2700
#define ACTIVEX_MAGIC		2800
#define GOALPOSTING_MAGIC	2900
#define AGENTBOX_MAGIC		3000
#define AS_MAGIC			3100
#define AGENT_DAEMON_MAGIC  3200
#define AGENTS_MAGIC		3300


//////////////////////////////////////////////////////////////////
// New Window Messages

#define WM_PASSPROC WM_USER + CENTRAL_MAGIC + 1


//////////////////////////////////////////////////////////////////
// External Function Prototypes

int __cdecl OutOfMemory(size_t size);
void __cdecl ErrAndExit( int type = 0, TCHAR *errmsg = 0, 
								int line = 0, TCHAR *sourcefile = _T(""), int err = 0);
void __cdecl ErrAndExit( int type = 0, UINT rid = 0, 
								int line = 0, TCHAR *sourcefile = _T(""), int err = 0);
void __cdecl ErrAndExit2( int type, UINT rid, 
								int line, TCHAR *sourcefile, int err,...);
void __cdecl Info( UINT rid = 0, int line = 0, TCHAR *sourcefile = _T(""));
void __cdecl Info( TCHAR *msg,  int line = 0, TCHAR *sourcefile = _T(""));
void __cdecl Warn( UINT rid = 0,  int line = 0, TCHAR *sourcefile = _T(""));
void __cdecl Warn( TCHAR *errmsg = 0,  int line = 0, TCHAR *sourcefile = _T(""));

#endif


