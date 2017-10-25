// Header file for Options

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef OPTIONS_H
#define OPTIONS_H

#include "LyraDefs.h"

#ifndef OPTIONS_DLL
#include "LmAvatar.h"
#include "Central.h"
#endif

//////////////////////////////////////////////////////////////////
// Constants

const short min_volume = 1;
const short max_volume = 10;
const int MAX_IGNORELIST = 64;
const int MAX_STORED_ACCOUNTS = 8;

//////////////////////////////////////////////////////////////////
// Types

struct other_t // another player for the buddy list or ignore list
{
	TCHAR name[Lyra::PLAYERNAME_MAX];
};


enum network_type  {
	NO_NETWORK = 0,
	INTERNET = 1,
	LAN = 2,
	MODEM = 3,
};


//////////////////////////////////////////////////////////////////
// Options Dialog Box Procedures

BOOL CALLBACK OptionsDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);

struct options_t {
	TCHAR username[MAX_STORED_ACCOUNTS][Lyra::PLAYERNAME_MAX];
	TCHAR password[MAX_STORED_ACCOUNTS][Lyra::PASSWORD_MAX];
	int  account_index; // current index into above username/password arrays
	BOOL sound; // load sound drivers - out of game option only
	BOOL sound_active; // sound currently active - toggle in game
	BOOL music_active; // sound currently active - toggle in game
	BOOL reverse;	
	int network; // type of networking
	BOOL debug; // use debugging server on port 7599?
	char game_server[64]; // IP address of game server
	int  server_port; // port of game server
	BOOL welcome_ai;
//	BOOL create_character;
	int entrypoint; // which starting pos

// this conditional is done to avoid linking in the messages to the DLL 
#ifdef OPTIONS_DLL
	int avatar[2];
#else
	LmAvatar avatar; // player's avatar
#endif

	BOOL nametags; // show name tags over heads
	BOOL multiline; // allow multi-line messages
	float turnrate; // speed of turning
	int effects_volume; // sound volume 
	int music_volume; // music volume 
	int speech_color; // color of chat
	int message_color; // color of system message
	int bg_color; // color of chat background
	int num_bungholes; // ignore list
	int num_gm_effects;
	int pmare_type;		// type of pmare session; could be a continuation
	int pmare_start_type; // avatar of starting form for this session
	SYSTEMTIME pmare_session_start; // time last pmare session started
	int pmare_price; // price of last session
	short pmare_minutes_online; // stores minutes online, or chosen max value
	DWORD pmare_logout_time; // time for force logout for pmare
	short pmare_session_minutes; // # of minutes from previous sessions
	//DWORD UNIX_login_time; // UNIX time (secs since 1970) at login, from server
	//DWORD local_login_time; // result of timeGetTime/1000 at login (when UNIX_login_time is set)

	bool babble_filter;
	bool ignore_whispers;
	bool debug_state_mode;

	other_t bungholes[MAX_IGNORELIST]; // ignore list members
	BOOL exclusive = TRUE; // exclusive mode ('95 only)
	BOOL autoreject; // always reject all parties
	BOOL autorejoin; // always rejoin last party leader
	BOOL footsteps;  // play footstep sounds
	BOOL art_prompts; // show art prompts
	BOOL mouselook; // mouse movements turn player by default
	BOOL invertmouse; // inverts y-axis of movement during mouselook
	BOOL log_chat; // write out player chat to a log
	BOOL invulnerable; // gm only!
	BOOL autorun; // always run
	BOOL rw; // using roger wilco real time voice
	BOOL adult_filter; // filter out adult language
	BOOL extra_scroll; // add additional line of scrolling for 2000/ME
	//BOOL udp_proxy; // use UDP proxy on the server side
	int bind_local_tcp;
	int bind_local_udp;
	BOOL restart_last_location; // for Chinese Ul; log in at last location
	BOOL tcp_only = true; // use TCP only - for firewall/NAT/ICS/etc.

	int resolution; // 640 = 640x480, 800 = 800x600, 1024 = 1024x768

#ifdef UL_DEV
	int dev_server; // 1, 2, 3
	char custom_ip[64] = "127.0.0.1"; // IP of Custom Server
#endif

	TCHAR gamed_URL[64]; // URL of game server pointer
	TCHAR patch_URL[64]; // URL of patch file
};

#ifndef OPTIONS_DLL
TCHAR* RegPlayerKey(bool fBase);
void LoadInGameRegistryOptionValues(HKEY reg_key, bool force);
void LoadCharacterRegistryOptionValues(bool force);
void LoadCharacterRegistryOptionValues(HKEY reg_key, bool force);
void __cdecl SaveCharacterRegistryOptionValues(HKEY reg_key);
void __cdecl SaveInGameRegistryOptionValues(HKEY reg_key);
void __cdecl SaveInGameRegistryOptionValues(void);
#endif


#endif
