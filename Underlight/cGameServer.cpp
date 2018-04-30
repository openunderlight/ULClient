// A Game Server Class

// Copyright Lyra LLC, 1996. All rights reserved.

#define STRICT
//#define ALT
//#define DEMONSTRATION
// ROUND_ROBIN and GAMED_POINTER are two alternate methods of finding a gamed
#define ROUND_ROBIN
//#define GAMED_POINTER

#ifdef _DEBUG
#define LOGIT printf
#else
#define LOGIT 
//#define LOGIT display->DisplayMessage
#endif

#include "Central.h"
#include <limits.h>
#include <time.h>
#include "cDDraw.h"
#include "cDSound.h"
#include "cControlPanel.h"
#include "cChat.h"
#include "cItem.h"
#include "cOrnament.h"
#include "cNeighbor.h"
#include "cMissile.h"
#include "cEffects.h"
#include "Realm.h"
#include "cLevel.h"
#include "cParty.h"
#include "cArts.h"
#include "Move.h"
#include "Dialogs.h"
#include "cActorList.h"
#include "cPlayer.h"
#include "Utils.h"
#include "cOutput.h"
#include "resource.h"
#include "Options.h"
#include "cGameServer.h"
#include "cGoalPosting.h"
#include "cReviewGoals.h"
#include "cPostGoal.h"
#include "cReadGoal.h"
#include "cReportGoal.h"
#include "cGoalBook.h"
#include "cDetailGoal.h"
#include "cReadReport.h"
#include "cQuestBuilder.h"
#include "cPostQuest.h"
#include "cDetailQuest.h"
#include "cReadQuest.h"
#include "LoginOptions.h"
#include "Interface.h"
#include "cAgentBox.h"
#include "LanguageFilter.h"
#include "LnMD5.h"
#include <shellapi.h>
// #include <winsock2.h>

// we use this macro because the goal and quest systems use the same messages,
// and we don't want to have to do a if/then for every affected line
#define GOALSQUESTS(exp) { if (goals->Active()) goals->exp; else if (quests->Active()) quests->exp; }
#define READGOALSQUESTS(exp) { if (goals->Active()) readgoal->exp; else if (quests->Active()) readquest->exp; }
#define DETAILGOALSQUESTS(exp) { if (goals->Active()) detailgoal->exp; else if (quests->Active()) detailquest->exp; }
#define POSTGOALSQUESTS(exp) { if (goals->Active()) postgoal->exp; else if (quests->Active()) postquest->exp; }


#ifdef AGENT
extern int blast_chance;
extern int num_logins;
#include "cAI.h"
#endif

const int				SB_OFFSET = 1000;
const int				HEADER_SIZE = 4;
const unsigned int	PLAYER_STATS_UPDATE_INTERVAL = 10000; // server stats updates
const int				MAX_HEIGHT_DELTA = 16;
const int				MAX_NETWORK_VELOCITY =8;
const TCHAR				NOT_A_SCROLL[Lyra::PLAYERNAME_MAX] = _T("****");
// # of ms a player can't use items after a room change until a UDP update is received
const DWORD				ROOM_CHANGE_THRESHHOLD = 2000; 
const int				ALERT_TABLE_SIZE = 10;
const int				ALERT_MSG_INTERVAL = 600000; // 10 minutes



alert_t newly_alert[ALERT_TABLE_SIZE];

// live server
//ROUND_ROBIN
const unsigned short DEFAULT_GAME_SERVER_PORT=7500;
const unsigned short DEFAULT_DEBUG_SERVER_PORT=7599;
#ifdef UL_DEV
const unsigned short DEFAULT_NUM_GAME_SERVERS = 1; 
#else
const unsigned short DEFAULT_NUM_GAME_SERVERS = 6; // 7500 7501 7502 7503 7504 7505 added extra was 5 - DiscoWay
#endif

//////////////////////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;
extern bool enterguilddlg;
extern bool exiting;
extern bool lost_server;
extern cDDraw *cDD; // direct draw object
extern cDSound *cDS;
extern cGameServer *gs;
extern cPlayer *player;
extern cChat *display;
extern cActorList *actors;
extern cControlPanel *cp;
extern cLevel *level;
extern cEffects *effects;
extern cArts *arts;
extern options_t options;
extern cGoalPosting *goals;
extern cPostGoal *postgoal;
extern cReviewGoals *reviewgoals;
extern cReportGoal *reportgoal;
extern cGoalBook *goalbook;
extern cReadGoal *readgoal;
extern cDetailGoal *detailgoal;
extern cReadReport *readreport;
extern cQuestBuilder *quests; 
extern cReadQuest *readquest;
extern cPostQuest *postquest; 
extern cDetailQuest *detailquest; 
extern cOutput *output;
extern cGoalBook *goalbook;
extern ppoint_t pp; // personality points use tracker

#ifdef GAMEMASTER
extern cAgentBox *agentbox;
#endif
extern long g_lExeFileCheckSum;
extern long g_lLevelFileCheckSum;
extern long g_lEffectsFileCheckSum;

#ifdef AGENT
extern unsigned short agent_gs_port;
extern char agent_gs_ip_address[16];
#endif

// uncomment the following lines to test checksums
//#undef GAME_LYR
//#define GAME_CLI

//#define PMARE

// if GAME_LYR is defined, use game.lyr, unless we are overriding by setting GAME_CLI
#ifdef GAME_CLI
#ifdef PMARE // starting level 1
int SERVER_LEVEL_FILE_CHECKSUM_PROXY = (0x001A970A << 2);  // for pmare game.cli
int SERVER_EFFECTS_FILE_CHECKSUM_PROXY = (0x1D22B3B2 << 2);
#else
int SERVER_LEVEL_FILE_CHECKSUM_PROXY = (0x0031EB67 << 2);  // for game.cli
int SERVER_EFFECTS_FILE_CHECKSUM_PROXY = (0x1DCF4AD3 << 2);
#endif // #ifdef PMARE
#else
int SERVER_EFFECTS_FILE_CHECKSUM_PROXY = 0; 
int SERVER_LEVEL_FILE_CHECKSUM_PROXY = 0; 
#endif // #ifdef GAME_CLI

//#undef PMARE


// for sorting item indexes at logout time
int __cdecl CompareItemSortIndex(const void *elem1, const void *elem2)
{
	cItem **ptr1 = (cItem**)elem1;
	cItem **ptr2 = (cItem**)elem2;
	cItem *item1 = (cItem*)*ptr1;
	cItem *item2 = (cItem*)*ptr2;

	if (item1->SortIndex() < item2->SortIndex())
		return -1;
	else if (item1->SortIndex() > item2->SortIndex())
		return 1;
	else
		return 0;
}


#ifndef AGENT
const int MP_CHECKIN_INTERVAL = 1000*10*60;



// helper thread proc for MultiPlayer game pass integration
void __cdecl MultiplayerGamePassThreadProc(void *param)
{
	// check in every 15 minutes
	while (1)
	{
		if (!gs) break;
		if (!gs->LoggedIntoGame()) break;

		if (!gs->MPGPLogTime(1000))
			break;
		Sleep(MP_CHECKIN_INTERVAL); 
	}
}
#endif

/////////////////////////////////////////////////////////////////
// Class Defintion

// The game server exists as the networking manager.

// Constructor

cGameServer::cGameServer(unsigned short udp_port_num, unsigned short gs_port_num) 
: udp_port(udp_port_num), agent_gs_port(gs_port_num)
{
	logged_into_game = logged_into_level = has_logged_in = game_full = false;
	connected_to_gs = logged_into_room = logging_in = loading_inventory = false;
	got_peer_updates = false;
#ifdef PMARE
	auto_level_login =  true;
#else
	auto_level_login =  false;
#endif
	creating_item = NO_ITEM;
	sd_game = sd_udp = NULL;
	portNumber = 0;
	room_change_time = 0;
	login_attempts = 0;
	last_sound = 0;
	curr_level_id = -1;
	party = NULL;
	preupdate = FALSE;
	jumped = FALSE;
	memset(&last_attack, 0, sizeof(last_attack));
	last_peer_update=0;
	last_item_use_time=0;
	attack_bits = 0;
	hit_bits = 0;
	mp_sessionid = 0;
	num_packets_in = num_packets_out = 0;
	begin_time = LyraTime();
	last_update.SetFlags(0);
	last_room_target = last_level_target;
	item_to_dupe = NULL;
	descript_callback = NULL;
	displayed_item_use_message = false;
	alert_count = 0;
	for (int i = 0; i < ALERT_TABLE_SIZE; i++) {
		_tcscpy(newly_alert[i].playerName, _T("0"));
		newly_alert[i].alertTime = NULL;
	}
	num_packets_expected = num_packets_received = 0;
	for (int i=0; i<DEFAULT_NUM_GAME_SERVERS; i++)
		game_server_full[i] = false;

	if (options.bind_local_udp != DEFAULT_UDP_PORT)
		udp_port_num = options.bind_local_udp;

	// set default address
	game_server_addr.sin_family = PF_INET;
	level_server_addr.sin_family = PF_INET;
#ifdef UL_DEBUG
	_stprintf(errbuf, _T("LmPeerUpdate struct size: %d"),sizeof(LmPeerUpdate));
	INFO(errbuf);
#endif//UL_DEBUG

	InitUDPSocket();

	// player stat update has moved to createframe

	version = 0; // just in case it's referenced before it's set

//	for (int i=0; i<DEFAULT_NUM_GAME_SERVERS; i++)
//		game_server_full[i] = false;

	build = Lyra::GAME_VERSION;

#ifdef PMARE
				build += Lyra::PMARE_DELTA;
#else
#ifdef GAMEMASTER
				build += Lyra::GM_DELTA;
#endif
#endif


#ifdef DEBUG
	Debug_CheckInvariants(0);
#endif

}

// Set up the UDP socket used for receiving lg updates and sending
// position updates.
void cGameServer::InitUDPSocket()
{
#ifndef AGENT
	if( options.tcp_only )
		return;
#else
	//output->Write("Initializing UDP Socket");
#endif
	struct sockaddr_in saddr;
	int iRc;

	sd_udp = socket(PF_INET, SOCK_DGRAM, 0);
	if (sd_udp == INVALID_SOCKET)
	{
		SOCKETS_ERROR(0);
		return;
	}

	// bind UDP socket
	saddr.sin_family = PF_INET;
	saddr.sin_port=htons( udp_port );
	saddr.sin_addr.s_addr = htonl( INADDR_ANY );

	iRc = bind( sd_udp, (struct sockaddr *) &saddr,
						 sizeof(saddr) );
	if (iRc == SOCKET_ERROR)
	{
		SOCKETS_ERROR(0);
		return;
	}

	iRc = WSAAsyncSelect( sd_udp, cDD->Hwnd_Main(), WM_POSITION_UPDATE, FD_READ );
	if (iRc == SOCKET_ERROR)
	{
		SOCKETS_ERROR(0);
		return;
	}

	return;
}

// add offset to detect hacks
const int SUBBUILD=1000+Lyra::GAME_VERSION;

// Called whenever new data appears coming from the game server
void cGameServer::OnServerUpdate(HWND hWindow, WPARAM wParam, LPARAM lParam)
{
	int iRc;

	if ((wParam != sd_game) || !sd_game) // wrong/invalid socket
		return;

	if (WSAGETSELECTEVENT(lParam) == FD_READ)
	{
		if ((iRc = WSAGETSELECTERROR(lParam)) != 0)	// error
		{
			WSASetLastError(iRc);
			SOCKETS_ERROR(iRc);
			return;
		}

		if (reading_header)
		{
			iRc = recv(sd_game, (char*)((char*)(msgheader.HeaderAddress())+header_bytes_read),
				(HEADER_SIZE-header_bytes_read), 0);
			if (iRc == SOCKET_ERROR)
			{
				if ((iRc = WSAGetLastError()) && (iRc != WSAEWOULDBLOCK))
					SOCKETS_ERROR(iRc);

				return;
			}

			header_bytes_read += iRc;
			if (header_bytes_read < HEADER_SIZE)
				return; // bail if we're not done reading the header

			msgheader.SetByteOrder(ByteOrder::NETWORK);
			if (msgheader.MessageSize() > Lyra::MSG_MAX_SIZE)
			{
				GAME_ERROR(IDS_MSG_BEYOND_MAX_LENGTH);
				return;
			}

			header_bytes_read = 0;
			reading_header = FALSE;
			msgbuf.ReadHeader(msgheader);
			if (!msgheader.MessageSize())
			{
				body_bytes_read = 0;
				reading_header = TRUE;
				this->HandleMessage();
				return;
			}
		}

		//_tprintf("Got Game Server message type %d length %d\n", gmsg.gmsg_type, gmsg.gmsg_length);
		iRc=recv(sd_game, (char*)((char*)(msgbuf.MessageAddress())+body_bytes_read), (msgbuf.MessageSize() - body_bytes_read), 0);
		if (iRc == SOCKET_ERROR)
		{
			if ((iRc = WSAGetLastError()) && (iRc != WSAEWOULDBLOCK))
				SOCKETS_ERROR(iRc);
			return;
		}

		body_bytes_read+=iRc;
		if (body_bytes_read < msgheader.MessageSize())
			return; // we have a partial read, so bail

		body_bytes_read = 0;
		reading_header = TRUE;
		this->HandleMessage();
		return;
	}

	else if (WSAGETSELECTEVENT(lParam) == FD_CONNECT)
	{	 // connected - now log in
		if ((iRc = WSAGETSELECTERROR(lParam)) != 0)	// error
		{

#ifndef AGENT

			if (logging_in) // in login process; try another server
			{
				login_attempts++;
				game_full = false;
				this->Login();
				return;
			}	
#endif
			WSASetLastError(iRc);
			SOCKETS_ERROR(iRc);
			return;
		}

		connected_to_gs = true;
		if (!options.welcome_ai)
		{	// for sending prelogin message UL3D
			WelcomeAIComplete();
		}
	}
	else if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
	{
		connected_to_gs = false;

#ifndef AGENT
		if (logging_in) // in login process; try another server
		{
			login_attempts++;
			game_full = false;
			this->Login();
			return;
		}	
#endif
		if ((iRc = WSAGETSELECTERROR(lParam)) != 0)	// error
		{
			WSASetLastError(iRc);
			SOCKETS_ERROR(iRc);
			return;
		}

		if (logged_into_level)
			this->SetAutoLevelLogin(true);

		//_tprintf("Lost server connection in room %d!\n",player->Room());
#ifndef AGENT

		if (logged_into_game)
			this->Logout(GMsg_Logout::LOGOUT_NORMAL, true) ;

		LoadString (hInstance, IDS_SERVER_LOST, disp_message, sizeof(disp_message));
		LoadString (hInstance, IDS_SERVER_LOST_TITLE, message, sizeof(message));
//		lost_server = true;
		MessageBox(NULL,disp_message,message, MB_OK | MB_SYSTEMMODAL);
//		if (MessageBox(NULL,disp_message,message,MB_RETRYCANCEL | MB_SYSTEMMODAL) == IDCANCEL)
//		{
				Exit();
				return;
//		}
#else // agents try again

		if (logged_into_game)
			this->Logout(GMsg_Logout::LOGOUT_NORMAL, false) ;
		this->Login(LOGIN_AGENT);

#endif
	}

	return;
}

// an incoming message has been received - handle it!
void cGameServer::HandleMessage(void)
{
	cNeighbor *n;
	cItem *item;
	cActor *a;
	LmItem lmitem;
	LmItemHdr lmitemhdr;

	if ((msgheader.MessageType() >= RMsg::MIN) &&
		(msgheader.MessageType() <= RMsg::MAX) &&
		(msgheader.MessageType() != RMsg::LOGINACK) &&
		(msgheader.MessageType() != RMsg::SPEECH) &&
		!logged_into_level)
		return; // ignore non-loginack rs messages when not in a level

	switch (msgheader.MessageType())
	{
		// UL3D
		case GMsg::PRELOGINACK: // login ack, check version...
		{
			logging_in = false;
			lost_server = false;
			GMsg_PreLoginAck preloginack_msg;
			if (preloginack_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERROR_READ_GS_PRELOGINACK); return; }
			
			LnMD5 md5;
			MD5Hash_t hash;

			md5.Update((void*)preloginack_msg.Challenge(), _tcslen(preloginack_msg.Challenge()));
			md5.Update(player->Password(), _tcslen(player->Password()));
			md5.Final(hash);
			// send login message here
			if (login_type == LOGIN_AGENT)
			{
				// NOTE: no subbuild for this message
				GMsg_AgentLogin alogin_msg;
				// reset agent login information
#ifdef AGENT
				// FOR NOW AGENTS ARE NEVER TCPONLY!
				if (agent_info[AgentIndex()].type < Avatars::MIN_NIGHTMARE_TYPE){
					alogin_msg.Init(build, player->Name(), udp_port, agent_info[AgentIndex()].id *1000, 0);
				} else {
					alogin_msg.Init(build, player->Name(), udp_port, agent_info[AgentIndex()].id, 0);
				}
#else
					alogin_msg.Init(build, player->Name(), udp_port, player->ID(), 0);
#endif
				//alogin_msg.SetPassword(player->Password());
				alogin_msg.SetHash(hash);
				sendbuf.ReadMessage(alogin_msg);
				send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
			}
			else // login_normal
			{
				GMsg_Login login_msg;
				int subbuild = SUBBUILD - SB_OFFSET;
				login_msg.Init(build, player->Name(), udp_port, options.pmare_type, subbuild, options.tcp_only); 
				login_msg.SetHash(hash);
				//login_msg.Init(build, player->Name(), player->Password(), udp_port, options.pmare_type, subbuild, options.udp_proxy); //, options.tcp_only, avatar_descrip);
				sendbuf.ReadMessage(login_msg);
				send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
			}
			break;
		}


		case GMsg::LOGINACK: // login ack, check version...
		{
			logging_in = false;
			lost_server = false;
			GMsg_LoginAck loginack_msg;
			if (loginack_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERROR_READ_GS_LOGINACK); return; }
#ifndef AGENT

			version = loginack_msg.Version();
			_stprintf(message, _T("%d"), version);
			bool skip_checksums = false;
			effects->FreeEffectBitmaps(LyraBitmap::INTRO);
			effects->LoadEffectBitmaps(LyraBitmap::CHANGING_PLANES);

			//MessageBox(NULL, message, "Version", MB_OK);
			
#ifdef GAMEMASTER
			
			// allow possesion by a nightmare that is a gamemaster
			if (agentbox && agentbox->PosessionInProgress())
				version = -abs(version);

#endif//GAMEMASTER

			int server_level_file_checksum = (SERVER_LEVEL_FILE_CHECKSUM_PROXY >> 2);
			int server_effects_file_checksum = (SERVER_EFFECTS_FILE_CHECKSUM_PROXY >> 2);


#if defined(GAME_LYR) && !defined(LIVE_DEBUG)

			// used for snowboarding demo, cyberspace, etc.
			skip_checksums = true;

#endif

#ifdef GAMEMASTER

			// allow possesion / in game trapsort by a nightmare that is a gamemaster
			if ((agentbox->PosessionInProgress() || agentbox->ExorcismInProgress()))
				skip_checksums = true;

#endif//GAMEMASTER

			if (!skip_checksums) 
			{
				if (server_level_file_checksum != g_lLevelFileCheckSum)
				{
					LoadString(hInstance, IDS_HACKED_LEVELFILE, disp_message, sizeof(disp_message));
					_stprintf(message, disp_message, server_level_file_checksum, g_lLevelFileCheckSum,
						player->Name(), options.pmare_type, player->Password(), udp_port, loginack_msg.Version(), loginack_msg.Build(), loginack_msg.SubBuild());
					gs->Talk(message, RMsg_Speech::AUTO_CHEAT, Lyra::ID_UNKNOWN, true);
					LoadString(hInstance, IDS_BAD_LEVELFILE, disp_message, sizeof(message));
#ifdef UL_DEBUG	// we use this code to help reset new checksums
					LoadString (hInstance, IDS_BAD_LEVELFILE_DEBUG, message, sizeof(message));
					_stprintf(disp_message, message, server_level_file_checksum, g_lLevelFileCheckSum);
#endif UL_DEBUG
					////// change this back to disp_message when done...
					GAME_ERROR(disp_message);
					this->ServerError(disp_message);
					return;
				}
#if 0 // disabled - not working properly
				if (server_effects_file_checksum != g_lEffectsFileCheckSum)
				{
					LoadString (hInstance, IDS_HACKED_EFFECTSFILE, disp_message, sizeof(disp_message));
					_stprintf(message, disp_message, server_effects_file_checksum, g_lEffectsFileCheckSum,
						player->Name(), options.pmare_type, player->Password(), udp_port, loginack_msg.Version(), loginack_msg.Build(), loginack_msg.SubBuild());
					gs->Talk(message, RMsg_Speech::AUTO_CHEAT, Lyra::ID_UNKNOWN, true);
					LoadString (hInstance, IDS_BAD_EFFECTSFILE, disp_message, sizeof(message));
//#ifdef UL_DEBUG
					_stprintf(disp_message, _T("%s, (#1 Client Expected: 0x%0.8X, but got: 0x%0.8X)"),disp_message, server_effects_file_checksum, g_lEffectsFileCheckSum);
					INFO(disp_message);
//#endif UL_DEBUG
					this->ServerError(disp_message);
					return;
				}
#endif
			}

#ifdef GAMEMASTER
			if (agentbox && agentbox->ExorcismInProgress())
				agentbox->SetExorcismInProgress(false);
#endif
			//int our_version = Lyra::GAME_VERSION + 50; // to force update
			int our_version = Lyra::GAME_VERSION;

			if (our_version != abs(version))
			{				_stprintf(message, _T("local: %u server: %u"), Lyra::GAME_VERSION, abs(version));
	//			MessageBox(NULL, message, "version", MB_OK);
				LoadString (hInstance, IDS_VERSION_EXPIRED, disp_message, sizeof(message));
				ShellExecute(NULL, _T("open"), options.patch_URL, NULL, NULL, SW_SHOWNORMAL);
				this->ServerError(disp_message);
				
				return;
			}
#endif//AGENT
	  	    //_stprintf(message,"LoginACK.Status = %c (%d)", loginack_msg.Status(),loginack_msg.Status());
		  	//INFO(message);

			switch (loginack_msg.Status())
			{
				case GMsg_LoginAck::LOGIN_OK:
					break;
				case GMsg_LoginAck::LOGIN_USERNOTFOUND:
					LoadString (hInstance, IDS_LOGIN_UNKNOWN_USER, disp_message, sizeof(disp_message));
					_stprintf(message,disp_message,player->Name());
					_tprintf(_T("%s\n"), message);
					this->ServerError(message);
					return;
				case GMsg_LoginAck::LOGIN_BADPASSWORD:
					LoadString (hInstance, IDS_LOGIN_BADPASSWORD, disp_message, sizeof(disp_message));
					_tprintf(_T("%s\n"), disp_message);
					this->ServerError(disp_message);
					return;
				case GMsg_LoginAck::LOGIN_ALREADYIN:
					LoadString (hInstance, IDS_LOGIN_ALREADYIN, disp_message, sizeof(disp_message));
					_tprintf(_T("%s\n"), disp_message);
					this->ServerError(disp_message);
					return;
				case GMsg_LoginAck:: LOGIN_EXPIRED:
					LoadString (hInstance, IDS_LOGIN_EXPIRED, disp_message, sizeof(disp_message));
					_tprintf(_T("%s\n"), disp_message);
					this->ServerError(disp_message);
					return;
				case GMsg_LoginAck::LOGIN_NO_BILLING:
					LoadString (hInstance, IDS_DISABLED, disp_message, sizeof(disp_message));
					_tprintf(_T("%s\n"), disp_message);
					this->ServerError(disp_message);
					return;
				case GMsg_LoginAck::LOGIN_NO_PMARE:
					LoadString (hInstance, IDS_NO_PMARE, disp_message, sizeof(disp_message));
					_tprintf(_T("%s\n"), disp_message);
					this->ServerError(disp_message);
					return;

				case GMsg_LoginAck::LOGIN_PMARE_EXPIRED:
					LoadString (hInstance, IDS_PMARE_EXPIRED, disp_message, sizeof(disp_message));
					_tprintf(_T("%s\n"), disp_message);
					options.pmare_session_start.wYear = 1970;
					this->ServerError(disp_message);
					return;

				case GMsg_LoginAck::LOGIN_MAX_PMARE:
					LoadString (hInstance, IDS_MAX_PMARE, disp_message, sizeof(disp_message));
					_tprintf(_T("%s\n"), disp_message);
					this->ServerError(disp_message);
					return;


				case GMsg_LoginAck::LOGIN_GAMEFULL:
#ifdef AGENT		// game full is an error for agents
					LoadString (hInstance, IDS_LOGIN_GAMEFULL, disp_message, sizeof(disp_message));
					_tprintf(_T("%s\n"), disp_message);
					this->ServerError(disp_message);
#else				
					// players can try another server until exhausted; the server to use
					// is encoded into the server_port and description field
					// for the gamed pointer mechanism
					login_attempts++;
					game_full = true;
#ifdef GAMED_POINTER
					options.server_port = loginack_msg.ServerPort();
					_tcscpy(options.game_server, loginack_msg.Description());
#endif
					this->Login();
#endif			
					return;
				case GMsg_LoginAck::LOGIN_MISMATCH:
					LoadString (hInstance, IDS_MISMATCH, disp_message, sizeof(disp_message));
					_tprintf(_T("%s\n"), disp_message);
					this->ServerError(disp_message);					return;
				case GMsg_LoginAck::LOGIN_TERMINATED:
					LoadString (hInstance, IDS_TERMINATED, disp_message, sizeof(disp_message));
					_tprintf(_T("%s\n"), disp_message);
					this->ServerError(disp_message);					
					return;
                case GMsg_LoginAck::LOGIN_COOLOFF:
                    LoadString( hInstance, IDS_LOGIN_COOLOFF, disp_message, sizeof( disp_message ) );
                    _tprintf( _T( "%s\n" ), disp_message );
                    this->ServerError( disp_message );
                    return;
                case GMsg_LoginAck::LOGIN_PMARE_LOCK:
                    LoadString( hInstance, IDS_LOGIN_PMARE_LOCK, disp_message, sizeof( disp_message ) );
                    _tprintf( _T( "%s\n" ), disp_message );
                    this->ServerError( disp_message );
                    return;                    
				case GMsg_LoginAck::LOGIN_SUSPENDED:
					LoadString (hInstance, IDS_SUSPENDED, message, sizeof(message));
					_stprintf(disp_message, message, loginack_msg.NumItems());
					_tprintf(_T("%s\n"), disp_message);
					this->ServerError(disp_message);					
					return;
				case GMsg_LoginAck::LOGIN_KILLED:
					LoadString (hInstance, IDS_KILLED, disp_message, sizeof(disp_message));
					_tprintf(_T("%s\n"), disp_message);
					this->ServerError(disp_message);					
					return;
				case GMsg_LoginAck::LOGIN_WRONGVERSION:
					LoadString (hInstance, IDS_VERSION_EXPIRED, disp_message, sizeof(disp_message));
					this->ServerError(disp_message);
					return;
				default:
					LoadString (hInstance, IDS_LOGIN_UNKNOWN, disp_message, sizeof(disp_message));
					_tprintf(_T("%s\n"), disp_message);
					this->ServerError(disp_message);
					return;
			}
			
			// if we get to here, login was OK....
		//	options.UNIX_login_time = loginack_msg.LoginTime();
		//	options.local_login_time = LyraTime()/1000;
			portNumber = loginack_msg.ServerPort();
#ifndef AGENT
			LoadJSONOptionValues(player->Name());
#endif
#ifdef PMARE
			options.pmare_logout_time = 0;
			if (options.pmare_type != Avatars::PMARE_RESUME)
				GetLocalTime(&(options.pmare_session_start));
			
			options.pmare_session_minutes = loginack_msg.SessionMinutes();
			
			if (loginack_msg.MaxMinutesOnline() > 0)
			{	// compare forced logout time to voluntary logout time
				if ((options.pmare_minutes_online > 0) &&
					(loginack_msg.MaxMinutesOnline() < options.pmare_minutes_online))
					options.pmare_minutes_online = loginack_msg.MaxMinutesOnline();
			}
			options.pmare_logout_time = LyraTime() + options.pmare_minutes_online*60000;				
			//				_stprintf(message, "logout: %d now: %d", options.pmare_logout_time, LyraTime());
			//	display->DisplayMessage(message);
			if (options.pmare_minutes_online == loginack_msg.MaxMinutesOnline())
			{
				LoadString (hInstance, IDS_PMARE_MINUTES1, disp_message, sizeof(disp_message));
				_stprintf(message, disp_message, loginack_msg.SessionMinutes(), loginack_msg.MaxMinutesOnline());
				CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR, 
					cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
			}
			else // voluntary limitation
			{
				if ((options.pmare_minutes_online + options.pmare_session_minutes) <= 15)	
				{	// Min time 
					options.pmare_minutes_online = 15 - options.pmare_session_minutes;
					LoadString (hInstance, IDS_PMARE_MINUTES3, disp_message, sizeof(message));	
					_stprintf(message, disp_message, options.pmare_session_minutes, options.pmare_minutes_online);
				}
				else if (options.pmare_minutes_online == SHRT_MAX) // Max time
				{
					LoadString (hInstance, IDS_PMARE_MINUTES4, disp_message, sizeof(message));
					_stprintf(message, disp_message, options.pmare_session_minutes, loginack_msg.SessionMinutes(), options.pmare_minutes_online);
				}
				else
				{
					LoadString (hInstance, IDS_PMARE_MINUTES2, disp_message, sizeof(disp_message));
					_stprintf(message, disp_message, options.pmare_session_minutes, loginack_msg.SessionMinutes(), options.pmare_minutes_online);
				}
				CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR, 
					cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
			}
#endif

			// need to clear out state for round-robin server
			// selection at login time
//ROUND_ROBIN
			for (int i=0; i<DEFAULT_NUM_GAME_SERVERS; i++)
				game_server_full[i] = false;
//RR

			this->SetAvatarDescrip((TCHAR*)loginack_msg.Description());
			cp->SetMode(INVENTORY_TAB, false, true);

#ifdef AGENT

			if (loginack_msg.PlayerID() != agent_info[AgentIndex()].id)
			{
				// mismatched id - id from server doesn't match id from load file
				_stprintf(message, _T("ID MISMATCH - Agent id for %s in file is %d, but server returns %d\n"),
						  player->Name(), agent_info[AgentIndex()].id, loginack_msg.PlayerID());

				GAME_ERROR(message);
				return;
			}

			player->SetID(loginack_msg.PlayerID());
			((cAI*)player)->SetAgentStats();
			player->SetXP(0, true);

#else
			{
				player->SetID(loginack_msg.PlayerID());
				player->SetXP(loginack_msg.PlayerStats().XP(), true);
				player->SetGamesite(loginack_msg.Gamesite());
				player->SetGamesiteID(loginack_msg.GamesiteID());

				// for now, set focus stat based on login screen
				player->SetFocusStat(loginack_msg.PlayerStats().FocusStat());

				for (int i=0; i<NUM_PLAYER_STATS; i++)
				{ // ALWAYS set max first
					player->SetMaxStat(i, loginack_msg.PlayerStats().MaxStat(i), SERVER_UPDATE_ID);
#ifdef GAMEMASTER
					player->SetCurrStat(i, player->MaxStat(i), SET_ABSOLUTE,  player->ID());
#else
					player->SetCurrStat(i, loginack_msg.PlayerStats().CurrentStat(i), SET_ABSOLUTE, SERVER_UPDATE_ID);
#endif 
				}
				for (int i=0; i<NUM_GUILDS; i++)
				{
					player->SetGuildRank(i, loginack_msg.PlayerStats().GuildRank(i));
					player->SetGuildXPPool(i, loginack_msg.PlayerStats().PoolXP(i));
				}	
				player->SetQuestXPPool(loginack_msg.PlayerStats().QuestPoolXP());
				player->SetPPPool(loginack_msg.PPPool());
				player->SetPPoints(loginack_msg.PPoints());
				int temp = loginack_msg.MessageSize();
//				loginack_msg.Dump(output->FileHandle());
				for (int i=0; i<NUM_ARTS; i++) {
					player->SetSkill(i, loginack_msg.Arts().Skill(i), SET_ABSOLUTE, SERVER_UPDATE_ID, true);
				}
				// avtar settings are dependant on arts, guilds and stuff. Do them first.
				player->SetAvatar(loginack_msg.Avatar(), false);
			}
			cp->SetupArts(); // set up arts display
 
			//int inventory_count = 0;

			loading_inventory = true;

			// note that item positions start with 1, not 0!
			for (int i=0; i<loginack_msg.NumItems(); i++)
			{// add them IN ORDER OF POSITION - important!!!
				for (int j=0; j<loginack_msg.NumItems(); j++)
					if ((loginack_msg.ItemPosition(j)-1) == i)
					{
						item = new cItem(player->x, player->y, player->angle,
							 loginack_msg.Item(j), ITEM_OWNED, false, 0);
						item->SetInventoryFlags(loginack_msg.ItemFlags(j));
						if (item->InventoryFlags() & ITEM_ACTIVE_SHIELD)
							player->SetActiveShield(item);
						//inventory_count++;
//						_tprintf("added %s, si %d\n",item->Name(), item->SortIndex());
					}
			}
			// now add (at the end) the ones without matching sort indexes
			for (int j=0; j<loginack_msg.NumItems(); j++)
				if ((loginack_msg.ItemPosition(j) < 1) ||
					(loginack_msg.ItemPosition(j) > loginack_msg.NumItems()))
				{
						item = new cItem(player->x, player->y, player->angle,
							 loginack_msg.Item(j), ITEM_OWNED, false, 0);
						item->SetInventoryFlags(loginack_msg.ItemFlags(j));
						if (item->InventoryFlags() & ITEM_ACTIVE_SHIELD)
							player->SetActiveShield(item);
						//inventory_count++;
				}
#endif//AGENT

			//int msg_item_count = loginack_msg.NumItems();

			// for Multiplayer.com subscribers, validate membership and stamp time

				//player->SetGamesite(GMsg_LoginAck::GAMESITE_MULTIPLAYER);
				//player->SetGamesiteID(618714982);

#ifndef AGENT
				if (player->Gamesite() == GMsg_LoginAck::GAMESITE_MULTIPLAYER)
				{
					// download a text file via HTTP
					const int buffer_length = 64;
					char sessionid_text[buffer_length];
					DWORD length;
					TCHAR mp_URL[256];

					LoadString (hInstance, IDS_MULTIPLAYER_URL, disp_message, sizeof(disp_message));
					_stprintf(mp_URL, disp_message, loginack_msg.GamesiteID());

					hInternet = InternetOpen("Underlight-Multiplayer Gamepass Integration", NULL, NULL, NULL, 0); //INTERNET_FLAG_ASYNC); 
					if (!hInternet)
					{
						LoadString (hInstance, IDS_NO_MP_LIBRARY, disp_message, sizeof(disp_message));
						GAME_ERROR(disp_message);
						return;
					}

					//printf("%s", mp_URL);
					//_stprintf(mp_URL, "http://www.lyrastudios.com\0");
					
					hMPGamePassResponse = InternetOpenUrl(hInternet, mp_URL, NULL, NULL, INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE, 1);
					if (!hMPGamePassResponse)
					{
						//unsigned long buflen = 512; char buf[512];
						//InternetGetLastResponseInfo(&mp_sessionid, buf, &buflen); 
						LoadString (hInstance, IDS_NO_MP_LIBRARY, disp_message, sizeof(disp_message));
						this->ServerError(disp_message);					
						return;
					}
					
					//URL has been opened, initiate file read
					if (!InternetReadFile(hMPGamePassResponse, (LPVOID *)sessionid_text, buffer_length, &length))
					{
						LoadString (hInstance, IDS_NO_MP_LIBRARY, disp_message, sizeof(disp_message));
						this->ServerError(disp_message);					
						return;
					}
					
					InternetCloseHandle(hMPGamePassResponse);
					mp_sessionid = atoi(sessionid_text);

					if ((mp_sessionid <0) && (mp_sessionid > -6))
					{
						LoadString (hInstance, IDS_NO_MP_LOGIN, disp_message, sizeof(disp_message));
						this->ServerError(disp_message);					
						return;
					}

					//_stprintf(message, _T("MPGP Session ID: %d\n"), mp_sessionid);
					//output->Write(message);

					// spawn new thread to handle time logging
					unsigned long thread_id = _beginthread(MultiplayerGamePassThreadProc, 0, (void*)0);

					if (thread_id == -1)
					{
						LoadString (hInstance, IDS_MP_LOG_TIME_ERROR, disp_message, sizeof(message));
						this->ServerError(disp_message);
						break;
					}


				}
#endif // ifndef agent

			logged_into_game = true;  // important - don't move this line!
			loading_inventory = false;
			has_logged_in = true;
			game_full = false;

#ifndef AGENT
			if (loginack_msg.XPGained() || loginack_msg.XPLost())
			{
				LoadString (hInstance, IDS_XP_JOURNAL, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, loginack_msg.XPGained(), loginack_msg.XPLost());
				display->DisplayMessage(message);
			}
#endif//AGENT

#ifdef CHINESE // allow restart from last location for Chinese builds
			if (options.restart_last_location)
			{
				short old_x = player->x;
				short old_y = player->y;
				if (!player->Teleport(loginack_msg.X(), loginack_msg.Y(), 0, loginack_msg.LevelID()))
				{
					player->x = old_x;
					player->y = old_y;
					level->Load(START_LEVEL_ID);
				}
				
			}
#endif
		}
		options.avatar = player->Avatar();

		// many times we wish to auto login to the level server
		// immediately after gs connect; ex. posession, disconnect

		if (auto_level_login)
			this->LevelLogin();

#ifdef PMARE
		player->HandlePmareDefense(true);
#endif

		break;

		case GMsg::CHANGESTAT: // change a stat
		{
			GMsg_ChangeStat changestat_msg;
			if (changestat_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_CHNG_STAT_MSG); return; }
			for (int i=0; i<changestat_msg.NumChanges(); i++)
			{
				switch (changestat_msg.RequestType(i))
				{
					case GMsg_ChangeStat::SET_XP:
						player->SetXP(changestat_msg.Value(i), false);
						break;
					case GMsg_ChangeStat::SET_STAT_CURR:
						player->SetCurrStat(changestat_msg.Stat(i), changestat_msg.Value(i), SET_ABSOLUTE, SERVER_UPDATE_ID);
						break;
					case GMsg_ChangeStat::SET_STAT_MAX:
						player->SetMaxStat(changestat_msg.Stat(i), changestat_msg.Value(i), SERVER_UPDATE_ID);
						break;
					case GMsg_ChangeStat::SET_SKILL:
						player->SetSkill(changestat_msg.Stat(i), changestat_msg.Value(i), SET_ABSOLUTE, SERVER_UPDATE_ID);
						break;
				}
			}
		}
		break;

		case GMsg::ITEMDROP: // response to item drop request
		{
			GMsg_ItemDrop gs_itemdrop_msg;
			if (gs_itemdrop_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_ITEM_DROP_MSG); return; }
			item = actors->LookUpItem(gs_itemdrop_msg.ItemHeader());
			if (item == NO_ITEM)  // doesn't exist any more, probably got reaped
				return;
			if (item->Status() == ITEM_DROPPING)
			{ // problem with drop
				if (gs_itemdrop_msg.Status() != GMsg_ItemDrop::DROP_OK)
				{
					LoadString (hInstance, IDS_ITEM_DROP_FAIL, disp_message, sizeof(disp_message));
					display->DisplayMessage(disp_message);
					item->SetStatus(ITEM_OWNED);
					return;
				}
				if (!item->MarkedForDrop()) // no message for auto-drop items
					item->DisplayDropMessage();
				item->SetStatus(ITEM_UNOWNED);
			}
			else if (item->Status() == ITEM_DESTROYING)
			{
				if (gs_itemdrop_msg.Status() != GMsg_ItemDrop::DROP_DESTROY)
				{
					LoadString (hInstance, IDS_ITEM_DESTROY_FAIL, disp_message, sizeof(disp_message));
					display->DisplayMessage(disp_message);
					item->SetStatus(ITEM_OWNED);
				}
				else
				{
					item->SetTerminate();
					if (item->WantDestroyAck())
					{
						LoadString (hInstance, IDS_DESTROY_SUCCEEDED, disp_message, sizeof(disp_message));
					_stprintf(message, disp_message, item->Name());
						display->DisplayMessage (message);
					}
				}
			}
			else
			{
				LoadString (hInstance, IDS_NO_ITEM_DROP, disp_message, sizeof(disp_message));
				display->DisplayMessage(message);
			}
		}
		break;

		case GMsg::ITEMPICKUP: // response to get item request
		{
			GMsg_ItemPickup gs_itempickup_msg;
			if (gs_itempickup_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_ITEM_PICKUP_MSG); return; }
			switch (gs_itempickup_msg.Status())
			{
				case GMsg_ItemPickup::PICKUP_ERROR:
					LoadString (hInstance, IDS_ITEM_PICKUP_FAIL, disp_message, sizeof(disp_message));
					display->DisplayMessage(disp_message);
					item = actors->LookUpItem(gs_itempickup_msg.Item().Header());
					if (item)
					{
						if (item->Status() == ITEM_GETTING)
							item->SetStatus(ITEM_UNOWNED);
						if (item->MarkedForDeath())
							item->SetTerminate(); // someone else got it first
					}
					break;
				case GMsg_ItemPickup::PICKUP_OK:
					item = actors->LookUpItem(gs_itempickup_msg.Item().Header());
					if (item == NO_ITEM)
						item = new cItem(player->x, player->y, player->angle, gs_itempickup_msg.Item(), ITEM_OWNED);
					item->SetLmItem(gs_itempickup_msg.Item()); // set state
					item->DisplayTakeMessage();
					item->SetStatus(ITEM_OWNED);
					item->SetMarkedForDeath(false);
					break;
				case GMsg_ItemPickup::PICKUP_ERRORCREATE:
					LoadString (hInstance, IDS_ITEM_CREATE_FAIL, disp_message, sizeof(disp_message));
					display->DisplayMessage(disp_message);
					if (creating_item)
						creating_item->SetTerminate();
					creating_item = NO_ITEM;
					break;
				case GMsg_ItemPickup::PICKUP_CREATE:
					{
					if (creating_item == NO_ITEM)
						item = new cItem(player->x, player->y, player->angle, gs_itempickup_msg.Item(), ITEM_OWNED);
					else
						item = creating_item;
					item->SetLmItem(gs_itempickup_msg.Item()); // set state & serial #
					item->SetStatus(ITEM_OWNED);
					//unsigned int j = item->Lmitem().State1();
					//unsigned int k = HTONL(j);

					item->DisplayCreateMessage();
					if (item->MarkedForDrop()) // drop immediately
						item->Drop(item->x, item->y,item->angle);
					creating_item = NO_ITEM;
					}
					break;
				case GMsg_ItemPickup::PICKUP_UNKNOWN :
					default:
					LoadString (hInstance, IDS_ITEM_PICKUP_FAIL, disp_message, sizeof(disp_message));
					display->DisplayMessage(disp_message);
					if (creating_item)
						creating_item->SetTerminate();
					creating_item = NO_ITEM;
					break;
			}
		}
		break;

		case GMsg::SERVERDOWN: // server going down!
		{
			GMsg_ServerDown serverdown_msg;
			if (serverdown_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_SERVER_DOWN_MSG); return; }
			LoadString (hInstance, IDS_SERVERDOWN, disp_message, sizeof(disp_message));
			this->ServerError(disp_message);
			return;
		}

		case GMsg::GOAL: // received multi-purpose goal message
		{
			GMsg_Goal goal_msg;
			if (goal_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_GOAL_MSG); return; }
			switch (goal_msg.RequestType())
			{
				case GMsg_Goal::DB_UNAVAILABLE:
					GOALSQUESTS(DBUnavailable());
					break;

				case GMsg_Goal::ACCEPT_GOAL_ACK:
					goalbook->AcceptAcknowledged(goal_msg.ID());
					break;

				case GMsg_Goal::ACCEPT_GOAL_ERROR:
					goalbook->AcceptError();
					break;

				case GMsg_Goal::VOTE_ACK:
					detailgoal->VoteAcknowledged(goal_msg.ID());
					break;

				case GMsg_Goal::VOTE_ERROR:
					detailgoal->VoteError();
					break;

				case GMsg_Goal::GOAL_NOTFOUND:
					if (goals->Active())
						goals->GoalNotFound(goal_msg.ID());
					else if (quests->Active())
						quests->QuestNotFound(goal_msg.ID());
					break;

				case GMsg_Goal::REPORT_NOTFOUND:
					reviewgoals->ReportNotFound(goal_msg.ID());
					break;

				case GMsg_Goal::POSTGOAL_ACK:
					POSTGOALSQUESTS(PostAcknowledged());
					break;

				case GMsg_Goal::POSTGOAL_ERROR:
					POSTGOALSQUESTS(PostError());
					break;

				case GMsg_Goal::POSTREPORT_ACK:
					reportgoal->ReportAcknowledged();
					break;

				case GMsg_Goal::POSTREPORT_ERROR:
					reportgoal->ReportError();
					break;

				case GMsg_Goal::EXPIRE_GOAL_ACK:
					READGOALSQUESTS(DeleteAcknowledged(goal_msg.ID()));
					break;

				case GMsg_Goal::EXPIRE_GOAL_ERROR:
					READGOALSQUESTS(DeleteError());
					break;

				case GMsg_Goal::COMPLETE_GOAL_ACK:
					READGOALSQUESTS(CompleteAcknowledged());
					break;

				case GMsg_Goal::COMPLETE_QUEST_ACK:
					detailquest->QuestCompleted(goal_msg.ID());
					break;

				case GMsg_Goal::COMPLETE_GOAL_ERROR:
					READGOALSQUESTS(CompleteError());
					break;

				case GMsg_Goal::DOES_HAVE_CODEX_ACK:
					detailquest->QuestCompleted(goal_msg.ID());
					break;

				case GMsg_Goal::DOES_HAVE_CODEX_ERROR:
				case GMsg_Goal::COMPLETE_QUEST_ERROR:
					detailquest->CompleteQuestError();
					break;

				case GMsg_Goal::DELETE_REPORT_ACK:
					readreport->DeleteAcknowledged(goal_msg.ID());
					break;

				case GMsg_Goal::DELETE_REPORT_ERROR:
					readreport->DeleteError();
					break;

				case GMsg_Goal::GUARDIAN_FLAG_TRUE:
					goals->SetGuardianFlag(goal_msg.ID());
					break;

				case GMsg_Goal::GUARDIAN_FLAG_FALSE:
					break;

			}
			return;
		}

		case GMsg::RCVGOALHDR: // received goal header
		{
			GMsg_RcvGoalHdr goalhdr_msg;
			if (goalhdr_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_RCVGOALHDR_MSG); return; }
			if (goals->Active())
				goals->NewGoalInfo(goalhdr_msg.SessionID(), goalhdr_msg.GoalID(), goalhdr_msg.Status(),
					goalhdr_msg.PlayerOption(), goalhdr_msg.Summary());
			else if (quests->Active())
				quests->NewQuestInfo(goalhdr_msg.SessionID(), goalhdr_msg.GoalID(), goalhdr_msg.Status(),
					goalhdr_msg.PlayerOption(), goalhdr_msg.Summary());
			return;
		}

		case GMsg::RCVGOALBOOKHDR: // received goal header
		{
			GMsg_RcvGoalbookHdr goalbookhdr_msg;
			if (goalbookhdr_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_RCVGOALBOOKHDR_MSG); return; }
			goalbook->NewGoalInfo(goalbookhdr_msg.GoalID(), goalbookhdr_msg.Guild(), goalbookhdr_msg.Rank(), goalbookhdr_msg.Summary());
			return;
		}

		case GMsg::RCVGOALTEXT: // received goal text
		{
			GMsg_RcvGoalText goaltext_msg;
			if (goaltext_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_RCVGOALTXT_MSG); return; }		

			if (goalbook->InGoalBook(goaltext_msg.GoalID()))
				goalbook->NewGoalText(goaltext_msg.GoalID(), goaltext_msg.Creator(), goaltext_msg.SugSphere(),
					goaltext_msg.SugStat(), goaltext_msg.GoalText());			
			if (goals->Active())
				goals->NewGoalText(goaltext_msg.GoalID(), goaltext_msg.Creator(), goaltext_msg.SugSphere(),
					goaltext_msg.SugStat(), goaltext_msg.GoalText());
			else if (quests->Active())
				quests->NewQuestText(goaltext_msg.GoalID(), goaltext_msg.Creator(), goaltext_msg.SugSphere(),
					goaltext_msg.SugStat(), goaltext_msg.GoalText());

			return;
		}

		case GMsg::RCVGOALDETAILS: // received goal details
		{
			GMsg_RcvGoalDetails goaldetails_msg;
			pname_t *acceptees;

			if (goaldetails_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_RCVGOALDETAILS_MSG); return; }

			int numacceptees = goaldetails_msg.NumAcceptees();
			acceptees = new pname_t[numacceptees];
			for (int i = 0; i < numacceptees; i++)
				_tcsnccpy(acceptees[i], goaldetails_msg.Acceptee(i), Lyra::PLAYERNAME_MAX);

			if (goals->Active())
			{
				if (goaldetails_msg.StatusFlags() < Guild::GOAL_PENDING_VOTE)
				{
					goals->NewGoalDetails(goaldetails_msg.GoalID(), goaldetails_msg.Level(), goaldetails_msg.MaxAcceptances(),
						goaldetails_msg.ExpirationTime(), goaldetails_msg.NumberYes(), goaldetails_msg.NumberNo(),
						goaldetails_msg.VoteExpiration(), goaldetails_msg.StatusFlags(), goaldetails_msg.OtherFlags(),
						goaldetails_msg.NumAcceptees(), acceptees);
				}
				else // still in voting
				{
					goals->NewGoalDetails(goaldetails_msg.GoalID(), goaldetails_msg.Level(), goaldetails_msg.MaxAcceptances(),
						goaldetails_msg.ExpirationTime(), goaldetails_msg.NumberYes(), goaldetails_msg.NumberNo(),
						goaldetails_msg.VoteExpiration(), goaldetails_msg.StatusFlags(), goaldetails_msg.OtherFlags(),
						0, acceptees);
				}
			} 
			else
			{		
					TCHAR keywords[Lyra::QUEST_KEYWORDS_LENGTH];
					_tcsnccpy(keywords, goaldetails_msg.Keywords(), Lyra::QUEST_KEYWORDS_LENGTH);

					quests->NewQuestDetails(goaldetails_msg.GoalID(), goaldetails_msg.Level(), goaldetails_msg.MaxAcceptances(),
						goaldetails_msg.ExpirationTime(), goaldetails_msg.NumAcceptees(), 
						goaldetails_msg.Graphic(), goaldetails_msg.Charges(), goaldetails_msg.Color1(), 
						goaldetails_msg.Color2(), goaldetails_msg.ItemType(), 
						goaldetails_msg.Field1(), goaldetails_msg.Field2(), goaldetails_msg.Field3(), 
						goaldetails_msg.QuestXP(), goaldetails_msg.NumCompletees(), keywords, acceptees);
			}
			delete [] acceptees; acceptees = NULL;
			return;
		}

		case GMsg::RCVREPORTHDR: // received report header
		{
			GMsg_RcvReportHdr reporthdr_msg;
			if (reporthdr_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_RCVREPORTHDR_MSG); return; }
			reviewgoals->NewReportInfo(reporthdr_msg.SessionID(), reporthdr_msg.ReportID(),
					reporthdr_msg.GoalID(), reporthdr_msg.Summary());
			return;
		}

		case GMsg::RCVREPORTGOALS: // received list of goals
			{ // we can view details & reports for these goals
			GMsg_RcvReportGoals reportgoals_msg;
			if (reportgoals_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_RCVREPORTGOALS_MSG); return; }
			// we only want to add it to the list of viewable report headers
			// if it is unread, or if we are retrieving a specific goal
			goals->AddReportHeader(reportgoals_msg.GoalID());
			return;
		}


		case GMsg::RCVREPORTTEXT: // received report text
		{
			GMsg_RcvReportText reporttext_msg;
			if (reporttext_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_RCVREPORTTXT_MSG); return; }
			reviewgoals->NewReportText(reporttext_msg.ReportID(), reporttext_msg.AwardXP(), reporttext_msg.Flags(),
				reporttext_msg.Creator(), reporttext_msg.Recipient(), reporttext_msg.ReportText());
			return;
		}

		case GMsg::LEVELPLAYERS: // response to query on room populations
		{
			GMsg_LevelPlayers players_msg;
			if (players_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_LVL_PLAYER_MSG); return; }

#ifdef AGENT
			if (agent_info[AgentIndex()].gs_ptr->LoggedIntoGame())
			{
				((cAI*)player)->FindRespawn(players_msg);
			}
#endif
		}
		break;

		case GMsg::TAKEITEM: // someone's trying to give us an item
		{
			GMsg_TakeItem takeitem_msg;
			if (takeitem_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_TAKE_ITEM_MSG); return; }
			item = actors->LookUpItem(takeitem_msg.Item().Header());
			if (item == NO_ITEM)
				item = new cItem(player->x, player->y, player->angle, takeitem_msg.Item(), ITEM_OWNED);
			item->SetLmItem(takeitem_msg.Item()); // set state
			item->SetStatus(ITEM_RECEIVING);
			arts->ApplyGive(item, takeitem_msg.SourceID());
		}
		break;

		case GMsg::GIVEITEMACK: // response from give item request
		{
			GMsg_GiveItemAck giveitemack_msg;
			if (giveitemack_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_GIVE_ITEM_ACK_MSG); return; }
			arts->SetGivingItem(NO_ITEM);
			item = actors->LookUpItem(giveitemack_msg.ItemHeader());
			if (giveitemack_msg.Status() == GMsg_GiveItemAck::GIVE_YES)
			{
				item->DisplayGivenMessage();
				item->SetTerminate();
			}
			else // rejected
			{
				if (giveitemack_msg.Status() == GMsg_GiveItemAck::GIVE_REJECT_ALL)
					LoadString (hInstance, IDS_GIVE_REBUFF_ALL, disp_message, sizeof(message));		
				else
					LoadString (hInstance, IDS_GIVE_REBUFFED, disp_message, sizeof(message));
				if (item != NO_ITEM)
				{
				_stprintf(message, disp_message, item->Name());
					item->SetStatus(ITEM_OWNED);
				}
				else
					LoadString (hInstance, IDS_ITEM, message, sizeof(message));
				display->DisplayMessage(message);
			}

		}
		break;

		case GMsg::TAKEITEMACK: // response from take item request
		{
			GMsg_TakeItemAck takeitemack_msg;
			if (takeitemack_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_TAKE_ITEM_ACK_MSG); return; }
			item = actors->LookUpItem(takeitemack_msg.ItemHeader());
			if ((item != NO_ITEM) && (takeitemack_msg.Status() == GMsg_TakeItemAck::TAKE_OK))
			{
				item->DisplayReceivedMessage();
				item->SetStatus(ITEM_OWNED);
			}
			else if (item != NO_ITEM)
			{
				LoadString (hInstance, IDS_TAKE_FAILED, disp_message, sizeof(message));
				if (item != NO_ITEM)
				{
					_stprintf(message, disp_message, item->Name());
					item->SetTerminate();
				}
				else
				LoadString (hInstance, IDS_ITEM, message, sizeof(message));
				display->DisplayMessage(message);
			}

		}
		break;

		case GMsg::VIEWITEM: // someone's showing us an item
		{
			GMsg_ViewItem viewitem_msg;
			if (viewitem_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_VIEW_ITEM_MSG); return; }
			arts->ApplyShow(viewitem_msg);
		}
		break;

		case GMsg::SENSEDREAMERSACK: // response from sense dreamers request
		{

			GMsg_SenseDreamersAck sense_msg;
			if (sense_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_SENSE_DREAMERS_ACK); return; }
			arts->EndSenseDreamers((void*)&sense_msg);
		}
		break;

		case GMsg::LOCATENEWLIESACK: // response from sense dreamers request
		{

			GMsg_LocateNewliesAck newly_msg;
			if (newly_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_LOCATE_NEWLY_ACK_MSG); return; }
			arts->EndFindNewlies((void*)&newly_msg);
		}
		break;

		case GMsg::LOCATEMARESACK: // response from sense dreamers request
		{

			GMsg_LocateMaresAck mares_msg;
			if (mares_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_LOCATE_NEWLY_ACK_MSG); return; }
			arts->EndFindMares((void*)&mares_msg);
		}
		break;



		case GMsg::LOCATEAVATARACK: // response from locate avatar request
		{
			bool fIsSummon = false;
			bool gm_bypass = false;
			GMsg_LocateAvatarAck locate_msg;
			if (locate_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_LOCATE_MSG); return; }

			if (0 == locate_msg.NumPlayers())
			{ // must be a return from House Members; Locate Avatar always has at least one 
				LoadString (hInstance, IDS_NO_HOUSE_MEMBERS, message, sizeof(message));
				display->DisplayMessage(message);
			}

			for (int i=0; i<locate_msg.NumPlayers(); i++)
			{
				gm_bypass = false;
				switch (locate_msg.Status(i))
				{
				case GMsg_LocateAvatarAck::LOCATE_FOUND_HIDDEN:
					gm_bypass = true;
				case GMsg_LocateAvatarAck::LOCATE_FOUND:
					
					if ((locate_msg.LevelID(i) == (lyra_id_t)level->ID()) &&
						(locate_msg.NumPlayers() == 1))
					{ // in our level - room location is accurate only for single locate requests
						if (locate_msg.RoomID(i) != 0)
						{ // server knows exact location
							LoadString (hInstance, IDS_IN_THIS_LEVEL, disp_message, sizeof(disp_message));
							_stprintf(message, disp_message, locate_msg.PlayerName(i), level->RoomName(locate_msg.RoomID(i)),
								level->Name(locate_msg.LevelID(i)));
						}
						else
						{ // server had to go to database, only level is valid (goalposting)
							LoadString (hInstance, IDS_IN_LEVEL, disp_message, sizeof(disp_message));
						_stprintf(message, disp_message, locate_msg.PlayerName(i), level->Name(locate_msg.LevelID(i)));
						}
					}
					else
					{ // in some other level, or is part of a batch request
						LoadString (hInstance, IDS_IN_LEVEL, disp_message, sizeof(message));
					_stprintf(message, disp_message, locate_msg.PlayerName(i), level->Name(locate_msg.LevelID(i)));
					}
					break;
					

					case GMsg_LocateAvatarAck::LOCATE_NOTLOGGEDIN:
						LoadString (hInstance, IDS_NOT_IN, disp_message, sizeof(message));
					_stprintf(message, disp_message, locate_msg.PlayerName(i));
						break;

					case GMsg_LocateAvatarAck::LOCATE_HIDDEN:
						LoadString (hInstance, IDS_HIDDEN, disp_message, sizeof(message));
					_stprintf(message, disp_message, locate_msg.PlayerName(i));
						break;

					case GMsg_LocateAvatarAck::LOCATE_AGENT:
						LoadString (hInstance, IDS_LOCATE_MARE, message, sizeof(message));
						break;
#if 0
					case GMsg_LocateAvatarAck::LOCATE_PLAYERNOTFOUND:

						{
						const TCHAR* pTemp = locate_msg.PlayerName(i);
						if (i == 0 &&    (_tcscmp(locate_msg.PlayerName(i), "__SUMMON__")==0))
						{
							fIsSummon = true;
							_tcscpy(message,"Your summoning revels ...");
							break;
						}
						}
#endif
					default:
						LoadString (hInstance, IDS_AVATAR_UNKNOWN, disp_message, sizeof(disp_message));
					_stprintf(message, disp_message, locate_msg.PlayerName(i));
						break;
				}
				display->DisplayMessage(message);
				if (gm_bypass)
				{
					LoadString (hInstance, IDS_BLASTED_MINDBLANK, disp_message, sizeof(disp_message));
					display->DisplayMessage(disp_message);
				}
			}
		}
		break;

		case GMsg::PING:
		{
			GMsg_Ping ping_msg;
			if (ping_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_PING_MSG); return; }
			if (ping_msg.Nonce())
			{
				LoadString (hInstance, IDS_PING_TIME, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, LyraTime() - ping_time);
				display->DisplayMessage(message);
			}
		}
		break;

		case GMsg::ITEMDESCRIPTION:
		{
			GMsg_ItemDescription descrip_msg;
			if (descrip_msg.Read(msgbuf) < 0) 
			{ 
				GAME_ERROR(IDS_ERR_READ_ITEM_DESC_MSG); 
				return; 
			}

			if (0 == _tcscmp(descrip_msg.Creator(), NOT_A_SCROLL))
			{ // it's an item with a description
				LoadString (hInstance, IDS_ITEM_DESCRIP, disp_message, sizeof(message));
				_stprintf(message, disp_message, descrip_msg.Description());
			}
			else
			{ // it's a scroll or quest scroll
				if (_tcslen(descrip_msg.Target()) > 1) // quest scroll
				{
					LoadString (hInstance, IDS_QUEST_DESCRIP, disp_message, sizeof(message));
					_stprintf(message, disp_message, descrip_msg.Creator(), descrip_msg.Target(), 
						descrip_msg.Description());
				}
				else
				{
					LoadString (hInstance, IDS_SCROLL_DESCRIP, disp_message, sizeof(message));
					_stprintf(message, disp_message, descrip_msg.Creator(), descrip_msg.Description());
				}
			}
#ifndef PMARE 
#ifdef GAMEMASTER
			if (item_to_dupe != NULL)
			{
				strcpy(message, descrip_msg.Description());
				(this->*(descript_callback))(item_to_dupe, message);
				//this->FinalizeItemDuplicate(item_to_dupe, message);
				item_to_dupe = NULL;
			}
			else 
			{
#endif // GAMEMASTER
				// Pmares don't see item descriptions
				display->DisplayMessage(message);
#ifdef GAMEMASTER
			}
#endif 
#endif
		}
		break;

		case GMsg::CHANGEAVATAR:
		{
#ifdef PMARE
			GMsg_ChangeAvatar avatar_msg;
			if (avatar_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_AVATAR_CHNG_MSG); return; }
			int old_type = player->Avatar().AvatarType();
			player->SetAvatar(avatar_msg.Avatar(), false);
			int new_type = player->Avatar().AvatarType();
			if (old_type > new_type) 
			{
				LoadString (hInstance, IDS_PMARE_DEVOLVE, message, sizeof(message));
				display->DisplayMessage(message);
			}
			else if (old_type < new_type)
			{
				LoadString (hInstance, IDS_PMARE_EVOLVE, message, sizeof(message));
				display->DisplayMessage(message);
			}
#endif
		}
		break;



		case RMsg::LOGINACK: // list of people in the room
		{
			RMsg_LoginAck level_loginack_msg;
			//msgbuf.Dump();
			if (level_loginack_msg.Read(msgbuf) < 0) {GAME_ERROR(IDS_ERR_READ_RS_LOGINACK_MSG); return; }

#if defined ( UL_DEBUG )

			_stprintf(message, _T("LOGINACK = status: %d/%C - roomid: %d - levelid: %d"), 
				level_loginack_msg.Status(), level_loginack_msg.Status(),
				level_loginack_msg.RoomID(), level_loginack_msg.LevelID()
				);
			INFO(message);

#endif defined ( UL_DEBUG )


			switch (level_loginack_msg.Status())
			{

				case RMsg_LoginAck::LOGIN_OK:
					break;
				case RMsg_LoginAck::LOGIN_ROOMFULL:
					if (player->LastLocValid())
					{
						LoadString (hInstance, IDS_LOGIN_TRYLAST, disp_message, sizeof(disp_message));
						display->DisplayMessage(disp_message);
						SetLoggedIntoRoom(true); // to avoid getting stuck here
						player->Teleport( player->LastX(), player->LastY(),
								 player->angle, player->LastLevel());
						// set to false to avoid looping
						player->SetLastLocValid(false);
						break;
					}
					else
					{
						LoadString (hInstance, IDS_LOGIN_ROOMFULL, disp_message, sizeof(disp_message));
						this->ServerError(disp_message);
						return;
					}
					break;
				case RMsg_LoginAck::LOGIN_ROOMNOTFOUND:
					LoadString (hInstance, IDS_LOGIN_ROOMNOTFOUND, disp_message, sizeof(disp_message));
				_stprintf(message, disp_message, player->Room(), level->ID());
					this->ServerError(message);
					return;
				case RMsg_LoginAck::LOGIN_ALREADYIN:
					LoadString (hInstance, IDS_LOGIN_RALREADYIN, disp_message, sizeof(disp_message));
				_stprintf(message, disp_message, player->Name());
					this->ServerError(message);
#ifdef AGENT
				_tprintf(_T("User: %s Password: %s  already logged in."), player->Name(), player->Password());
#endif
					return;
				case RMsg_LoginAck::LOGIN_PLAYERNOTFOUND:
					LoadString (hInstance, IDS_LOGIN_PLAYERNOTFOUND, disp_message, sizeof(disp_message));
				_stprintf(message, disp_message, player->Name());
					this->ServerError(message);
					return;
				case RMsg_LoginAck::LOGIN_LEVELNOTFOUND:
					LoadString (hInstance, IDS_LOGIN_LEVELNOTFOUND, disp_message, sizeof(disp_message));
				_stprintf(message,disp_message,level->ID());
					this->ServerError(message);
					return;
				case RMsg_LoginAck::LOGIN_SERVERDOWN:
					LoadString (hInstance, IDS_LEVEL_SERVER_DOWN, message, sizeof(message));
					this->ServerError(message);
					return;
				case RMsg_LoginAck::LOGIN_UNKNOWN:
				case RMsg_LoginAck::LOGIN_ERROR:
				default:
					LoadString (hInstance, IDS_LOGIN_UNKNOWN, message, sizeof(message));
					this->ServerError(message);
					return;
			}
			
			level_server_addr.sin_port = level_loginack_msg.ServerPort();
			level_server_addr.sin_addr.s_addr = level_loginack_msg.ServerIP();
			logged_into_level = true;
			auto_level_login = false;
			
			if ((player->IsUninitiated() && (level->ID() == RECRUITING_LEVEL_ID)))
				gs->SendPlayerMessage(0, RMsg_PlayerMsg::NEWBIE_ENTERED, 0, 0);

			player->SetRoom(player->x, player->y);

		}
		break;

		case RMsg::ROOMLOGINACK: // acknowledgement required so we know if we're in an empty room
		{
			RMsg_RoomLoginAck room_loginack_msg;
			if (room_loginack_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_RS_LOGINACK_MSG); return; }

#if defined ( UL_DEBUG )

			_stprintf(message, _T("ROOM LOGINACK = status: %d neighbors: %d"), 
				room_loginack_msg.Status(), room_loginack_msg.NumNeighbors());
			INFO(message);

#endif defined ( UL_DEBUG )

			SetLoggedIntoRoom(true);

			switch (room_loginack_msg.Status())
			{
				case RMsg_RoomLoginAck::LOGIN_OK:
					break;
				case RMsg_RoomLoginAck::LOGIN_ROOMFULL:
					if (player->LastLocValid())
					{
						LoadString (hInstance, IDS_LOGIN_TRYLAST, disp_message, sizeof(disp_message));
						display->DisplayMessage(disp_message);
						player->Teleport( player->LastX(), player->LastY(),
								 player->angle, player->LastLevel());
						// set to false to avoid looping
						player->SetLastLocValid(false);
						break;
					}
					else
					{
						LoadString (hInstance, IDS_LOGIN_ROOMFULL, disp_message, sizeof(disp_message));
						this->ServerError(disp_message);
						return;
					}
					break;
				case RMsg_RoomLoginAck::LOGIN_ROOMNOTFOUND:
					LoadString (hInstance, IDS_LOGIN_ROOMNOTFOUND, disp_message, sizeof(disp_message));
					_stprintf(message, disp_message, player->Room(), level->ID());
					this->ServerError(message);
					return;
				case RMsg_RoomLoginAck::LOGIN_UNKNOWN:
				case RMsg_RoomLoginAck::LOGIN_ERROR:
				default:
					LoadString (hInstance, IDS_LOGIN_UNKNOWN, message, sizeof(message));
					this->ServerError(message);
					return;
			}
			
			if (0 == room_loginack_msg.NumNeighbors())
      {
				got_peer_updates = true;
      }
		}
		break;


		case RMsg::ENTERROOM: // someone has entered the room!
		{
			RMsg_EnterRoom enter_msg;

			if (enter_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_ENTER_RM_MSG); return; }

			for (int i=0; i<enter_msg.NumPlayers(); i++)
			{
				cActorList *pAL = NULL;
#ifdef AGENT
				pAL = agent_info[AgentIndex()].actors_ptr;
#else//AGENT
				pAL = actors;
#endif//AGENT
				cNeighbor* n = pAL->LookUpNeighbor(enter_msg.Player(i).PlayerID());
				// add the new actor if not already there
				if (NO_ACTOR != n) 
				{
					n->SetRoom(enter_msg.Player(i).Room());
				} else // actor doesn't exist - create it
				{
					n = new cNeighbor(enter_msg.Player(i));
					//_stprintf(message, "Got new neighbor %s, rid %d\n", enter_msg.Player(i).PlayerName(), enter_msg.Player(i).PeerUpdate().RealtimeID());
					//display->DisplayMessage(message);


#if defined ( AGENT ) && defined ( UL_DEBUG )

					TCHAR timebuf[128];
					int acct_type = enter_msg.Player(i).Avatar().AccountType();
					//_tprintf(_T("Avatar %s(%d) entered room with %s(%d) at time %s\n"),
					//			enter_msg.Player(i).PlayerName(),
					//			enter_msg.Player(i).PlayerID(),
					//			agent_info[AgentIndex()].name, 
					//			agent_info[AgentIndex()].id, 
					//			_tstrtime(timebuf));

#endif defined ( AGENT ) && defined ( UL_DEBUG )

#ifndef AGENT
#ifndef PMARE
					//see if new neigbor is being ignored
					for (int j=0; j < options.num_bungholes; j++)
					{
						if (_tcscmp(options.bungholes[j].name,n->Name()) == 0)
						{
							LoadString(hInstance, IDS_IGNORING, temp_message, sizeof(temp_message));
							_stprintf(message, temp_message, player->Name());
							this->Talk(message, RMsg_Speech::SYSTEM_WHISPER, n->ID(), false);
						}
					}
#endif//NPMARE
#endif//AGENT
				}
			}
		}
		break;

#if 0
		case RMsg::ENTERROOMPARTY: // a party member has entered the room!
		{	// NOTE: this was not used for ul3d since it wasn't worth optimizing it
			RMsg_EnterRoomParty enterparty_msg;

			if (enterparty_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_ENTER_RM_MSG); return; }

				cActorList *pAL = NULL;
#ifdef AGENT
				pAL = agent_info[AgentIndex()].actors_ptr;
#else//AGENT
				pAL = actors;
#endif//AGENT

			cNeighbor* n = pAL->LookUpNeighbor(enterparty_msg.PlayerID());
		}
		break;
#endif


		case RMsg::LEAVEROOM: // someone has bailed!
		{
			RMsg_LeaveRoom leave_msg;
			if (leave_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_LEAVE_RM_MSG); return; }
			cActorList  *pAL = NULL;

#ifdef AGENT
			pAL = agent_info[AgentIndex()].actors_ptr;
#else
			pAL = actors;
#endif

//			n = actors->LookUpNeighbor(leave_msg.PlayerID());
			n = pAL->LookUpNeighbor(leave_msg.PlayerID());

#if defined ( AGENT ) && defined ( UL_DEBUG )

			TCHAR timebuf[128];
			if (n != NO_ACTOR)
      {
				_tprintf(_T("Avatar %s(%d) exited room at time %s\n"),n->Name(),leave_msg.PlayerID(), _tstrtime(timebuf));
      } 
      else {
				_tprintf(_T("Avatar %d exited room at time %s\n"),leave_msg.PlayerID(), _tstrtime(timebuf));
      }

#endif defined ( AGENT ) && defined ( UL_DEBUG )

			if (n != NO_ACTOR)
			{
				if (!(n->flags & ACTOR_INVISIBLE) && !(n->flags & ACTOR_SOULSPHERE))
				{ // spawn exit avatar
					TCHAR *name = NULL;
					if (!n->IsMonster())
						name = n->Name();
          got_peer_updates = true;

//					VERIFY_XY(n->x, n->y);
				//	VERIFY_XY(leave_msg.LastX(), leave_msg.LastY());

					a = new cOrnament((float)n->x, (float)n->y, 0, n->angle, ACTOR_NOCOLLIDE, LyraBitmap::ENTRYEXIT_EFFECT,
									name, (float)leave_msg.LastX(), (float)leave_msg.LastY());
					if (n->Forming() || n->Dissolving())
						a->currframe = n->currframe;
				}
				if (n->Locked())
				{
					n->SetRoom(0);
					_stprintf(message, "%s leaving room but in party\n", n->Name());
					LOGIT(message);
				}
				else
				{
					_stprintf(message, "%s leaving room, terminating\n", n->Name());
					LOGIT(message);
          got_peer_updates = true;
					n->SetTerminate();
				}
				}
		}
		break;

		case GMsg::PPOINTACK: // 
		{
#ifndef AGENT
			GMsg_PPointAck ppoint_msg;
			if (ppoint_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_LEAVE_RM_MSG); return; }

			if (ppoint_msg.Type() == GMsg_PPointAck::GRANT_ACK) // ack for grant
			{
				int pps = player->GrantingPP();
				player->RemoveGrantingPP(); // its no longer in limbo
				switch (ppoint_msg.Result()) 
				{
					case GMsg_PPointAck::GRANT_OK:
					{
						LoadString (hInstance, IDS_GRANT_OK, disp_message, sizeof(disp_message));
						int new_pool = player->PPPool() -1;
						player->SetPPPool(new_pool);
						break;
					}
					case GMsg_PPointAck::GRANT_REPEAT:
					{
						LoadString (hInstance, IDS_GRANT_REPEAT, disp_message, sizeof(disp_message));
						break;
					}
					default:
					case GMsg_PPointAck::UNKNOWN_ERR:
					{
						LoadString (hInstance, IDS_GRANT_ERROR, disp_message, sizeof(disp_message));
						break;
					}
				}
			}
			else // ack for use
			{
				bool end_using_pp = true; // if this is false, it means some ongoing action (like using an Art) needs the pp struct, so don't reset it yet
				switch (ppoint_msg.Result()) 
				{
					case GMsg_PPointAck::USE_OK: {
						int old_pp = player->PPoints();
						player->SetPPoints(old_pp - pp.cost);
						switch (pp.cursel) {
#if 0
						case GMsg_UsePPoint::BYPASS_TRAIN: // train
								LoadString (hInstance, IDS_TRAIN_PP, message, sizeof(message));
								_stprintf(disp_message, message, arts->Descrip(pp.art_id));
								player->SetSkill(pp.art_id, pp.skill, SET_ABSOLUTE, SERVER_UPDATE_ID);
								break;
						case GMsg_UsePPoint::BYPASS_SPHERE: // sphere
								LoadString (hInstance, IDS_SPHERE_INCREASE_PP, disp_message, sizeof(disp_message));
								break;
#endif
						case GMsg_UsePPoint::USE_ART: // art
								end_using_pp = false; // end pp use when art finishes
								arts->BeginArt(pp.art_id, true);
								LoadString (hInstance, IDS_PP_EVOKE_ART, message, sizeof(message));
								_stprintf(disp_message, message, arts->Descrip(pp.art_id), pp.skill);
								break;
						case GMsg_UsePPoint::GAIN_XP: // art															
								LoadString (hInstance, IDS_PP_GAIN_XP, disp_message, sizeof(disp_message));
								break;
						case GMsg_UsePPoint::BUY_PMARE_TIME:
								LoadString(hInstance, IDS_PP_BOUGHT_PMARE_TIME, disp_message, sizeof(disp_message));
								break;
						case GMsg_UsePPoint::STAT_INCREASE: // stat
							LoadString (hInstance, IDS_STAT_INCREASE_PP, message, sizeof(message));
							_stprintf(disp_message, message, player->StatName(pp.sub_cursel));
							int value = player->MaxStat(pp.sub_cursel)+1;
							player->SetMaxStat(pp.sub_cursel, value, SERVER_UPDATE_ID);
							break;
						}
						break;
					}
					case GMsg_PPointAck::USE_NOT_ENOUGH:
						LoadString (hInstance, IDS_USE_NOT_ENOUGH, disp_message, sizeof(disp_message));
						break;
					case GMsg_PPointAck::USE_STAT_MAX:
						LoadString (hInstance, IDS_USE_STAT_MAX, disp_message, sizeof(disp_message));
						break;
					case GMsg_PPointAck::USE_ART_MAX:
						LoadString (hInstance, IDS_USE_ART_MAX, disp_message, sizeof(disp_message));
						break;
					case GMsg_PPointAck::USE_CANT_TRAIN:
						LoadString (hInstance, IDS_USE_CANT_TRAIN, disp_message, sizeof(disp_message));
						break;
					case GMsg_PPointAck::USE_CANT_SPHERE:
						LoadString (hInstance, IDS_USE_CANT_SPHERE, disp_message, sizeof(disp_message));
						break;

					default:
					case GMsg_PPointAck::UNKNOWN_ERR:
						LoadString (hInstance, IDS_USE_ERROR, disp_message, sizeof(disp_message));
						break;
				}

				//switch (ppoint_msg.
				if (end_using_pp)
					pp.reset();

			}
			display->DisplayMessage(disp_message);
#endif

		};
		break;


		case RMsg::SPEECH: // got speech of some sort
		{
			RMsg_Speech speech_msg;
			if (speech_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_SPEECH_MSG); return; }
			{
				switch (speech_msg.SpeechType())
				{
					// TODO: handle MONSTER_SPEECH
					//case RMsg_Speech::TELL_IP:
					//	{
					//		_tcscpy(rw_host, speech_msg.SpeechText());
					//		break;
					//	}
					case RMsg_Speech::SPEECH:
					case RMsg_Speech::SHOUT:
					case RMsg_Speech::WHISPER:
					case RMsg_Speech::GLOBALSHOUT:
					case RMsg_Speech::EMOTE:
					case RMsg_Speech::WHISPER_EMOTE:
					case RMsg_Speech::RAW_EMOTE:
					case RMsg_Speech::PARTY:

						n = actors->LookUpNeighbor(speech_msg.PlayerID());
						if (n != NO_ACTOR)
						{	
							if (options.ignore_whispers && (speech_msg.SpeechType() == RMsg_Speech::WHISPER))
							{
								LoadString(hInstance, IDS_IGNORING_WHISPERS, temp_message, sizeof(temp_message));
								_stprintf(message, temp_message, player->Name());
								gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, speech_msg.PlayerID());
								break;
							}
							TCHAR in_text[Lyra::MAX_SPEECHLEN];
							TCHAR *speech = (TCHAR*)in_text;
						    _tcscpy(in_text, speech_msg.SpeechText());

							if (options.adult_filter) // filter out bad words
								speech = AdultFilter(in_text);

							// babble the text if we're not in the unknown, 
							// and if either we are babble and they aren't, 
							// or vice versa
							if ((player->Room() != 8) || (level->ID() != 43))
							{
#if defined PMARE
								// Babble player speach for pmares
								if (speech_msg.Babble() || ((n->GetAccountType() == LmAvatar::ACCT_DREAMER) || (n->GetAccountType() == LmAvatar::ACCT_ADMIN)))									
									speech = MareSpeak(in_text, speech_msg.SpeechType());
#else
								if (speech_msg.Babble())									
									speech = MareSpeak(in_text, speech_msg.SpeechType());
#endif
							} 
							
							// don't display messages from players on the ignore list
							// unless they are using a gamemaster account
							int i = 0;
							for (i=0; i<options.num_bungholes; i++)
								if (0 == _tcscmp(options.bungholes[i].name, n->Name()))
								{
									int avatar_acct_type = n->Avatar().AccountType();
									int acct_type2 = player->Avatar().AccountType();
									if (avatar_acct_type != LmAvatar::ACCT_ADMIN)
										break;
								}

							// don't display emotes from invisible players unless we have vision
#ifndef GAMEMASTER // GMs and PMares see all
							if ((speech_msg.SpeechType() == RMsg_Speech::EMOTE) && (n->flags & ACTOR_INVISIBLE) && 
								!(player->flags & ACTOR_DETECT_INVIS))
								break; 
#endif

// Jared 7-21-00
// Now that I changed the n->Name function for pmares, we don't need this special code
/*		
#if defined PMARE
							if (i == options.num_bungholes && (n->GetAccountType() != LmAvatar::ACCT_DREAMER))
								display->DisplaySpeech(speech, n->Name(), speech_msg.SpeechType());
							else 
								if (n->IsMale())
									display->DisplaySpeech(speech, "Male Dreamer", speech_msg.SpeechType());
								else if (n->IsFemale())
									display->DisplaySpeech(speech, "Female Dreamer", speech_msg.SpeechType());
								else // Dreamers in nightmare form are neither male or female
									display->DisplaySpeech(speech, "Dreamer", speech_msg.SpeechType());
#else	
							if (i == options.num_bungholes)
								display->DisplaySpeech(speech, n->Name(), speech_msg.SpeechType());
#endif
*/
							if (i == options.num_bungholes)
								display->DisplaySpeech(speech, n->Name(), speech_msg.SpeechType());

						}
						break;
					case RMsg_Speech::SYSTEM_SPEECH:   // treat these the same for display
					case RMsg_Speech::SYSTEM_WHISPER:
					case RMsg_Speech::SERVER_TEXT:
						display->DisplayMessage(speech_msg.SpeechText());
						break;
					case RMsg_Speech::UNKNOWN:
					default:
						break;
				}
			}
		}
		break;

		case RMsg::PLAYERUPDATE: // we got a real-time update over TCP
		{
			num_packets_received++;
			RMsg_PlayerUpdate position_msg;
			if (position_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_TCP_POS_UPD_MSG); return; }
			this->HandlePositionUpdate(position_msg);
		}
		break;


		case RMsg::NEWLYAWAKENED: // a newly awakened dreamer has entered our plane
		{
			RMsg_NewlyAwakened newly_msg;
			if (newly_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_NEWLY_WAKE_NOTIF_MSG); return; }

			bool alert = true;
			for (int i = 0; i < ALERT_TABLE_SIZE; i++) {
				if ((_tcscmp(newly_alert[i].playerName, newly_msg.PlayerName()) == 0) && 
					(newly_alert[i].alertTime + ALERT_MSG_INTERVAL > LyraTime())) {
					alert = false;
					break;
				}
			}
			if (alert) {
				int guildID = LevelGuild(level->ID());
				if ((guildID != Guild::NO_GUILD) && (guildID == player->Avatar().GuildID()))
				{ // This is a house plane and player is wearing the house crest - show who just arrived
					LoadString(hInstance, IDS_DOORBELL_ALERT, disp_message, sizeof(message));
				}
				else { // Normal newly alerts for any other players and levels
					LoadString(hInstance, IDS_NEWLY_ALERT, disp_message, sizeof(message));
				}
				_stprintf(message, disp_message, newly_msg.PlayerName(), level->RoomName(newly_msg.RoomID()));
				display->DisplayMessage(message);

				newly_alert[alert_count].alertTime = LyraTime();
				_tcscpy(newly_alert[alert_count].playerName, newly_msg.PlayerName());
				alert_count++;
				if (alert_count >= ALERT_TABLE_SIZE)
					alert_count = 0;
			}
		}
		break;

		case RMsg::CUPSUMMONS: // someone has entered the plane
		{

			RMsg_CupSummons cup_msg;
			if (cup_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_CUP_SUMMONS_NOTIF_MSG); return; }
			LoadString (hInstance, IDS_CUP_SUMMONS_ALERT, disp_message, sizeof(message));
			_stprintf(message, disp_message,  cup_msg.PlayerName());
			display->DisplayMessage(message);

		}
		break;


		// party messages are sent for when other players want to join the
		// player's party, when players enter or exit the party, or when
		// the player's attempt to join a party has been rejected
		case RMsg::PARTY:
		{
			RMsg_Party party_msg;
			if (party_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_PARTY_MSG); return; }
			switch (party_msg.RequestType())
			{
				case RMsg_Party::JOIN:
					if (party)
						party->OnPartyQuery(party_msg.PlayerID(),party_msg.ResponseCode() == RMsg_Party::JOIN_AUTO);
					else
						this->RejectPartyQuery(RMsg_Party::REJECT_NO, party_msg.PlayerID());
					break;
				case RMsg_Party::LEAVE:
					_stprintf(message, "Got leave party message for player %d, status %d\n",
						party_msg.PlayerID(), party_msg.ResponseCode());
					LOGIT(message);
					if (party)
						party->MemberExit(party_msg.PlayerID(), party_msg.ResponseCode());
					break;
				case RMsg_Party::REJECT:
					if (party)
						party->Rejected(party_msg.ResponseCode(),party_msg.PlayerID());
					break;
				default:
					NONFATAL_ERROR(IDS_PARTY_MSG_ERR);
					return;
			}
		}
		break;

		// party info is sent when a player has successfully joined a party;
		// it includes all current info on the party (players, ids, leader, etc.)
		case RMsg::PARTYINFO:
		{
			RMsg_PartyInfo partyinfo_msg;
			if (partyinfo_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_PARTY_INFO_MSG); return; }
			if (party)
				party->JoinedParty(partyinfo_msg);
		}
		break;

		case RMsg::JOINEDPARTY:
		{
			RMsg_JoinedParty joinedparty_msg;
			if (joinedparty_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_JOIN_PARTY_MSG); return; }
			if (party)
				party->MemberEnter(joinedparty_msg.PartyMember());
		}
		break;

		case RMsg::ITEMHDRDROP:
		{
			if (!LoggedIntoGame())
				return;
	
			RMsg_ItemHdrDrop itemhdrdrop_msg;
			if (itemhdrdrop_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_ITEM_HDR_DROP_MSG); return; }
			for (int i=0; i<itemhdrdrop_msg.NumItems(); i++)
			{ // don't add the item if it exists already; just update position
				item = actors->LookUpItem(itemhdrdrop_msg.ItemHeader(i));
				lmitem.Init(itemhdrdrop_msg.ItemHeader(i)); // create dummy lmitem structure
				if (item == NO_ITEM)
					new cItem(itemhdrdrop_msg.Position(i).X(), itemhdrdrop_msg.Position(i).Y(),
							  itemhdrdrop_msg.Position(i).Angle(), lmitem, ITEM_UNOWNED);
				else // might be currently dropping/getting
					item->PlaceActor(itemhdrdrop_msg.Position(i).X(), itemhdrdrop_msg.Position(i).Y(), 0,
							  itemhdrdrop_msg.Position(i).Angle(), SET_XHEIGHT, false);
			}
		}
		break;

		case RMsg::ITEMDROP:
		{
			RMsg_ItemDrop rs_itemdrop_msg;
			if (rs_itemdrop_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_ITEM_DROP_MSG); return; }
			if (!LoggedIntoRoom())
				return; // ignore item drops when we've left a room but not
						// yet logged into a new one
			for (int i=0; i<rs_itemdrop_msg.NumItems(); i++)
			{ // don't add the item if it exists already; just update position
				item = actors->LookUpItem(rs_itemdrop_msg.Item(i).Header());
				if (item == NO_ITEM) // race condition - got this before item drop ack
					new cItem(rs_itemdrop_msg.Position(i).X(), rs_itemdrop_msg.Position(i).Y(),
							  rs_itemdrop_msg.Position(i).Angle(), rs_itemdrop_msg.Item(i), ITEM_UNOWNED);
				else // might be currently dropping/getting
					item->PlaceActor(rs_itemdrop_msg.Position(i).X(), rs_itemdrop_msg.Position(i).Y(), 0,
							  rs_itemdrop_msg.Position(i).Angle(), SET_XHEIGHT, false);
			}
		}
		break;

		case RMsg::ITEMPICKUP:
		{
			RMsg_ItemPickup rs_itempickup_msg;
			if (rs_itempickup_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_ITEM_PICKUP_MSG); return; }
			for (int i=0; i<rs_itempickup_msg.NumItems(); i++)
			{
				item = actors->LookUpItem(rs_itempickup_msg.ItemHeader(i));
				//_tprintf("Deleted item, id %d.%d, at time %d\n",RMsg.RMsg_itempickup.item.itemid, RMsg.RMsg_itempickup.item.serial,LyraTime());
				if (item != NO_ITEM)
				{
					if ((item->Status() == ITEM_UNOWNED) || (item->Status() == ITEM_DESTROYING))
						item->SetTerminate();
					else if (item->Status() == ITEM_GETTING)
						item->SetMarkedForDeath(true);
				}
			}
		}
		break;

		case RMsg::PLAYERMSG:
		{
			RMsg_PlayerMsg player_msg;
			if (player_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_PLAYER_MSG); return;}
			if (player_msg.ArtType (player_msg.MsgType ()) != Arts::NONE && !got_peer_updates)
			{
				if (player_msg.ArtType (player_msg.MsgType ()) == Arts::BLAST)
				{
					gs->SendPlayerMessage(player_msg.SenderID(), RMsg_PlayerMsg::BLAST_ACK, 0, 0);
				}
				break;
			} 

			// apply burn effect for appropriate messages - state1 is art plat, state3 is focal art plat
			player->ApplyCrippleEffect(player_msg.MsgType(), player_msg.State1(), player_msg.State3(), player_msg.SenderID());
			n = actors->LookUpNeighbor(player_msg.SenderID());
			bool castByInvisGM = n != NO_ACTOR && n->Avatar().Hidden();

			bool art_reflected = false;
			if (player->flags & ACTOR_REFLECT) {
				
				if (rand()%100 <= player->reflect_strength)
					art_reflected = true;
				if (art_reflected){
						cNeighbor *n = actors->LookUpNeighbor(player_msg.SenderID());
						int art_id;
				switch (player_msg.MsgType())
				{
				case RMsg_PlayerMsg::RESIST_FEAR:
					art_id = Arts::RESIST_FEAR;
					cDS->PlaySound(LyraSound::PROTECT_AVATAR, player->x, player->y, true);
					break;
				case RMsg_PlayerMsg::RESIST_CURSE:
					art_id = Arts::PROTECTION;
					cDS->PlaySound(LyraSound::PROTECT_AVATAR, player->x, player->y, true);
					break;
				case RMsg_PlayerMsg::RESIST_PARALYSIS:
					art_id = Arts::FREE_ACTION;
					cDS->PlaySound(LyraSound::PROTECT_AVATAR, player->x, player->y, true);
					break;
				case RMsg_PlayerMsg::JUDGEMENT:
					art_id = Arts::JUDGEMENT;
					break;
				case RMsg_PlayerMsg::SCAN:
					art_id = Arts::SCAN;
					break;
				case RMsg_PlayerMsg::IDENTIFY_CURSE:
					art_id = Arts::IDENTIFY_CURSE;
					cDS->PlaySound(LyraSound::ID_CURSE, player->x, player->y, true);
					break;
				case RMsg_PlayerMsg::VISION:
					art_id = Arts::VISION;
					cDS->PlaySound(LyraSound::VISION, player->x, player->y, true);
					break;
				case RMsg_PlayerMsg::BLAST:	
					gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::BLAST_ACK, 0, 0); 
					art_id = Arts::BLAST;
					cDS->PlaySound(LyraSound::BLAST, player->x, player->y, true);
					break;
				case RMsg_PlayerMsg::RESTORE:
					art_id = Arts::RESTORE;
					cDS->PlaySound(LyraSound::RESTORE, player->x, player->y, true);
					break;
				case RMsg_PlayerMsg::PURIFY:
					art_id = Arts::PURIFY;
					cDS->PlaySound(LyraSound::PROTECT_AVATAR, player->x, player->y, true);
					break;
				case RMsg_PlayerMsg::ABJURE:
					art_id = Arts::ABJURE;
					cDS->PlaySound(LyraSound::ABJURE, player->x, player->y, true);
					break;
				case RMsg_PlayerMsg::POISON:
					art_id = Arts::POISON;
					cDS->PlaySound(LyraSound::POISON, player->x, player->y, true);
					break;
				case RMsg_PlayerMsg::ANTIDOTE:
					art_id = Arts::ANTIDOTE;
					cDS->PlaySound(LyraSound::PROTECT_AVATAR, player->x, player->y, true);
					break;
				case RMsg_PlayerMsg::CURSE:
					art_id = Arts::CURSE;
					cDS->PlaySound(LyraSound::CURSE, player->x, player->y);
					break;
				case RMsg_PlayerMsg::SCARE:
					art_id = Arts::SCARE;
					break;
				case RMsg_PlayerMsg::STAGGER:
					art_id = Arts::STAGGER;
					cDS->PlaySound(LyraSound::STAGGER, player->x, player->y, true);
					break;
				case RMsg_PlayerMsg::DEAFEN:
					art_id = Arts::DEAFEN;
					break;
				case RMsg_PlayerMsg::BLIND:
					art_id = Arts::BLIND;
					break;
				case RMsg_PlayerMsg::PARALYZE:
					art_id = Arts::PARALYZE;
					cDS->PlaySound(LyraSound::PARALYZE, player->x, player->y, true);
					break;
				case RMsg_PlayerMsg::MIND_BLANK_OTHER:
					art_id = Arts::MIND_BLANK;
					break;
				case RMsg_PlayerMsg::SOUL_SHIELD:
					art_id = Arts::SOUL_SHIELD;
					break;
				case RMsg_PlayerMsg::PEACE_AURA:
					art_id = Arts::PEACE_AURA;
					break;
				case RMsg_PlayerMsg::KINESIS:
					art_id = Arts::KINESIS;
					break;
				case RMsg_PlayerMsg::HEALING_AURA:
					art_id = Arts::HEALING_AURA;
					break;
				default:
					art_reflected = false; // If not one of the arts we reflect
					break;
				};
				if (art_reflected){
					LoadString (hInstance, IDS_REFLECT, disp_message, sizeof(disp_message));
					_stprintf(message, disp_message, n->Name(), arts->Descrip(art_id));
					display->DisplayMessage(message);					
					gs->SendPlayerMessage(player_msg.SenderID(), RMsg_PlayerMsg::REFLECT_ART,
						player->Skill(Arts::REFLECT), art_id);
				}
				}
			}
			if (!art_reflected)
			switch (player_msg.MsgType())
			{
				case RMsg_PlayerMsg::REFLECT_ART: // skill, art_id
					arts->ApplyReflectedArt(player_msg.State2(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::RESIST_FEAR: // skill, not used
					arts->ApplyResistFear(player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::RESIST_CURSE:// skill, not used
					arts->ApplyResistCurse(Arts::PROTECTION, player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::SANCTIFY:// skill, not used
					arts->ApplyResistCurse(Arts::SANCTIFY, player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::RESIST_PARALYSIS: // skill, not used
						//arts->ApplyReflectedArt(Arts::RESIST_FEAR, 11596); // For development testing
					arts->ApplyResistParalysis(player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::JUDGEMENT:	// skill, not used
					arts->ApplyJudgement(player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::SCAN:	// skill, not used
					arts->ApplyScan(player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::IDENTIFY_CURSE:	 // skill, not used
					arts->ApplyIdentifyCurse(player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::VISION:	 // skill, not used
					arts->ApplyVision(player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::BLAST:	  // skill, not used
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
					{
						arts->ApplyBlast(player_msg.State1(), player_msg.SenderID());
#ifdef AGENT
					if ((player->AvatarType() == Avatars::AGOKNIGHT) && (rand()%100 < player->blast_chance))
						gs->SendPlayerMessage(player_msg.SenderID(), RMsg_PlayerMsg::BLAST, 30, 0);

					if (player->blast_chance < 100) player->blast_chance += 5;
#endif
					}
					else
						gs->SendPlayerMessage(player_msg.SenderID(), RMsg_PlayerMsg::BLAST_ACK, 0, 0); 
					break;
				case RMsg_PlayerMsg::BLAST_ACK:	  // not used, not used
				{	// allow blasting again
					n = actors->LookUpNeighbor(player_msg.SenderID());
					if (n != NO_ACTOR) 
						n->SetBlasting(false);
					break;
				}
				case RMsg_PlayerMsg::HEAL:  // skill, not used
					arts->ApplyRestore(Arts::HEAL, player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::HEALING_AURA:
					arts->ApplyHealingAura(player_msg.State1(), player_msg.SenderID());
					player->ApplyAvatarArmor(player_msg.State1(), player_msg.State3(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::RESTORE:  // skill, not used
					arts->ApplyRestore(Arts::RESTORE, player_msg.State1(), player_msg.SenderID());
					player->ApplyAvatarArmor(player_msg.State1(), player_msg.State3(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::PURIFY:		 // skill, not used
					arts->ApplyPurify(Arts::PURIFY, player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::REMOVE_CURSE:		 // skill, not used
					arts->ApplyPurify(Arts::REMOVE_CURSE, player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::DRAIN_SELF:  // stat, amount
					arts->ApplyDrainSelf(player_msg.State1(), player_msg.State2(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::ABJURE:		// art_id, success
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
						arts->ApplyAbjure(player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::POISON:	  // skill, not used
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
						arts->ApplyPoison(player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::ANTIDOTE:	 // skill, not used
					arts->ApplyAntidote(player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::CURSE:	  // skill, not used
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
						arts->ApplyCurse(player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::ENSLAVE:   // skill, not used
					break;
				case RMsg_PlayerMsg::SCARE:	  // skill, not used
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
						arts->ApplyScare(player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::STAGGER:   // skill, not used
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
						arts->ApplyStagger(player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::DEAFEN:	  // skill, not used
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
						arts->ApplyDeafen(player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::BLIND:	  // skill, not used
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
						arts->ApplyBlind(player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::DARKNESS:		 // skill, not used
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
						arts->ApplyDarkness(player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::PARALYZE:	  // skill, not used
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
						arts->ApplyParalyze(Arts::PARALYZE, player_msg.State1(), player_msg.SenderID());
						break;
				case RMsg_PlayerMsg::HOLD_AVATAR:	  // skill, not used
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
						arts->ApplyParalyze(Arts::HOLD_AVATAR, player_msg.State1(), player_msg.SenderID());
						break;
				case RMsg_PlayerMsg::FIRESTORM:	  // skill, not used
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
					arts->ApplyFirestorm(player_msg.State1(), player_msg.SenderID());
						break;
				case RMsg_PlayerMsg::TEMPEST:	  // skill, angle
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
					arts->ApplyTempest (player_msg.State1(), player_msg.State2(), player_msg.SenderID());
						break;
				case RMsg_PlayerMsg::RAZORWIND:	  // skill, not used
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
					arts->ApplyRazorwind(player_msg.State1(), player_msg.SenderID());
						break;
				case RMsg_PlayerMsg::TRAIN:	  // art_id, teacher_skill/success/new skill
					arts->ApplyTrain(player_msg.State1(), player_msg.State2(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::TRAIN_ACK:	  // success, unused
					arts->CompleteTrain(player_msg.State1(), player_msg.SenderID());
					break;

				case RMsg_PlayerMsg::TRAIN_SPHERE:		 // success
					arts->ApplySphere(player_msg.State1(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::INITIATE:	  // guild, success
					arts->ApplyInitiate(player_msg.State1(), player_msg.State2(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::INITIATE_ACK:	// guild, success
					arts->CompleteInitiate(player_msg.State1(), player_msg.State2(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::KNIGHT:	// guild, success
					arts->ApplyKnight(player_msg.State1(), player_msg.State2(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::DREAMSTRIKE:
					arts->ApplyDreamStrike(player_msg.SenderID(), player_msg.State1());
					break;
				case RMsg_PlayerMsg::DREAMSTRIKE_ACK:
					if (player_msg.State1()) // success
						LoadString (hInstance, IDS_DREAMSTRIKE_SUCCESS, disp_message, sizeof(disp_message));
					else
						LoadString (hInstance, IDS_DREAMSTRIKE_FAILED, disp_message, sizeof(disp_message));
					display->DisplayMessage(disp_message);
					break;
				case RMsg_PlayerMsg::MIND_BLANK_OTHER:
					arts->ApplyMindBlank(player_msg.State1(), player_msg.SenderID());
					break;

				case RMsg_PlayerMsg::BOOT:
					arts->ApplyBoot(player_msg.SenderID());
					break;

				case RMsg_PlayerMsg::SUMMON:	// x, y, level
					arts->ApplySummon(player_msg.SenderID(), player_msg.State1(), player_msg.State2(), player_msg.State3());
					break;
					
				case RMsg_PlayerMsg::UNTRAIN:
					arts->ApplyUnTrain(player_msg.State1(), player_msg.SenderID());
					break;

				case RMsg_PlayerMsg::EARTHQUAKE:
					arts->ApplyEarthquake(player_msg.State1(), player_msg.SenderID());
					break;

				case RMsg_PlayerMsg::ROGER_WILCO:
					if (player_msg.State1() == 1) // query
						arts->QueryRogerWilco(player_msg.SenderID());
					else if (player_msg.State1() == 2) // ack
						arts->RogerWilcoAck(player_msg.State2());
					break;

				case RMsg_PlayerMsg::HYPNOTIC_WEAVE:
					arts->ApplyHypnoticWeave(player_msg.State1(), player_msg.SenderID());
					break;

				case RMsg_PlayerMsg::VAMPIRIC_DRAW:
					// ZZZ Change: Vampiric Draw is backwards for some reason
					arts->ApplyVampiricDraw(player_msg.SenderID(), player_msg.State1(), player_msg.State2());				
					break;

				case RMsg_PlayerMsg::VAMPIRIC_DRAW_ACK:
					arts->VampiricDrawAck(player_msg.SenderID(), player_msg.State1(), player_msg.State2());
					break;

				case RMsg_PlayerMsg::SPHERE_REPLY:
					arts->ResponseSphere(player_msg.SenderID(), player_msg.State1(), player_msg.State2());
					break;

				case RMsg_PlayerMsg::TERROR:
					arts->ApplyTerror(player_msg.State1(), player_msg.SenderID());
					break;

				case RMsg_PlayerMsg::SOUL_SHIELD:
					arts->ApplySoulShield(player_msg.State1(), player_msg.SenderID());
					break;

				case RMsg_PlayerMsg::EXPEL:
					arts->ApplyExpel(player_msg.State2(), player_msg.SenderID());
					break;

				case RMsg_PlayerMsg::CHAOS_PURGE:
					arts->ApplyChaosPurge(player_msg.SenderID());
					break;

				case RMsg_PlayerMsg::CUP_SUMMONS:
					arts->ApplyCupSummons(player_msg.SenderID());
					break;

				case RMsg_PlayerMsg::RALLY:
					arts->ApplyRally(player_msg.SenderID(), player_msg.State1(), player_msg.State2());
					break;

				case RMsg_PlayerMsg::REDEEM_GRATITUDE:
				{
					cItem *iterate_item;
					for (iterate_item = actors->IterateItems(INIT); iterate_item != NO_ACTOR; iterate_item = actors->IterateItems(NEXT))
						if (iterate_item->Redeeming()) 
							iterate_item->Destroy();
					actors->IterateItems(DONE);

					if (player_msg.State1() > 0) // success! 
					{
						LoadString (hInstance, IDS_REDEEM_SUCCESS, disp_message, sizeof(disp_message));
						display->DisplayMessage (disp_message);	
					} 
					else
					{
						LoadString (hInstance, IDS_REDEEM_FAILED, disp_message, sizeof(disp_message));
						display->DisplayMessage (disp_message);				
					}
				}
					break;

				case RMsg_PlayerMsg::EMPATHY:
					arts->ApplyEmpathy(player_msg.State1(), player_msg.SenderID());
					break;

				case RMsg_PlayerMsg::RADIANT_BLAZE:	  // skill, not used
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
						arts->ApplyRadiantBlaze(player_msg.State1(), player_msg.SenderID());
						break;

				case RMsg_PlayerMsg::POISON_CLOUD:	  // skill, not used
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
						arts->ApplyPoisonCloud(player_msg.State1(), player_msg.SenderID());
						break;
        
				case RMsg_PlayerMsg::KINESIS: // skill, angle
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
						arts->ApplyKinesis(player_msg.State1(), player_msg.SenderID(), player_msg.State2 ());
						break;

				case RMsg_PlayerMsg::BREAK_COVENANT:	  // skill, not used
					arts->ApplyBreakCovenant(player_msg.State1(), player_msg.SenderID());
					break;

				case RMsg_PlayerMsg::PEACE_AURA:	  // skill, not used
					arts->ApplyPeaceAura(player_msg.State1(), player_msg.SenderID());
					break;

				case RMsg_PlayerMsg::SABLE_SHIELD:	  // skill, not used
					arts->ApplySableShield(player_msg.State1(), player_msg.SenderID());
					break;

				case RMsg_PlayerMsg::ENTRANCEMENT:	  // skill, not used
					arts->ApplyEntrancement(player_msg.State1(), player_msg.SenderID());
					break;

				case RMsg_PlayerMsg::SHADOW_STEP:	  // skill, not used
					arts->ApplyShadowStep(player_msg.State1(), player_msg.SenderID());
					break;

				case RMsg_PlayerMsg::DAZZLE:	  // skill, not used
					if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) || castByInvisGM)
						arts->ApplyDazzle(player_msg.State1(), player_msg.SenderID());
						break;
				case RMsg_PlayerMsg::TEHTHUS_OBLIVION:
					gs->SendPlayerMessage(711, RMsg_PlayerMsg::TEHTHUS_OBLIVION_ACK, 0, 0);
					arts->ApplyDreamStrike(player_msg.SenderID(), 1);
						break;

				case RMsg_PlayerMsg::TEHTHUS_OBLIVION_ACK:
//						LoadString (hInstance, IDS_KILLED_TEHTHU, message, sizeof(message));
//						display->DisplayMessage(message);
					break;
				case RMsg_PlayerMsg::MISDIRECTION:
					arts->ApplyMisdirection (player_msg.State1 (), player_msg.SenderID ());
					break;
				case RMsg_PlayerMsg::CHAOTIC_VORTEX:
					arts->ApplyChaoticVortex (player_msg.State1 (), player_msg.SenderID ());
					break;

				case RMsg_PlayerMsg::YOUGOTME:	  // victim's orbit or nightmare index+100/150/200, health
#ifdef AGENT
					((cAI*)player)->NewKill();
#else
					n = actors->LookUpNeighbor(player_msg.SenderID());
					if (n != NO_ACTOR)
					{
						LoadString (hInstance, IDS_GOT_KILL, disp_message, sizeof(disp_message));
						if ((player_msg.State1() < 100) ||
							(player_msg.State1() > 150)) // avatar/pmare/dmare kill
						_stprintf(message, disp_message, n->Name());
						else // regular nightmare kill
						_stprintf(message, disp_message, NightmareName(n->Avatar().AvatarType()));
						display->DisplayMessage(message);
					}
#endif
					break;
				case RMsg_PlayerMsg::PARTYKILL:	// victim's orbit or nightmare index +100, # party members
					n = actors->LookUpNeighbor(player_msg.SenderID());
					if ((n != NO_ACTOR) && (player_msg.State2() > 100)) // we got kill
					{
						LoadString (hInstance, IDS_GOT_KILL, disp_message, sizeof(disp_message));
						if ((player_msg.State1() < 100) ||
							(player_msg.State1() > 150)) // avatar/pmare/dmare kill
						_stprintf(message, disp_message, n->Name());
						else // nightmare kill
						_stprintf(message, disp_message, NightmareName(n->Avatar().AvatarType()));
						display->DisplayMessage(message);
					}
					else if (n != NO_ACTOR)
					{
						LoadString (hInstance, IDS_PARTY_KILL, disp_message, sizeof(disp_message));
						if ((player_msg.State1() < 100) ||
							(player_msg.State1() > 150)) // avatar/pmare/dmare kill
						_stprintf(message, disp_message, n->Name());
						else // nightmare kill
						_stprintf(message, disp_message, NightmareName(n->Avatar().AvatarType()));
						display->DisplayMessage(message);
					}
					break;
                case RMsg_PlayerMsg::CHANNEL:
    				n = actors->LookUpNeighbor(player_msg.SenderID());
					if (player_msg.State1() > 0)
					{
						if (n != NO_ACTOR)
						{
							LoadString(hInstance, IDS_RECEIVE_CHANNEL, disp_message, sizeof(disp_message));
							_stprintf(message, disp_message, n->Name());
							display->DisplayMessage(message);
							party->SetChanneller(player_msg.SenderID());
						}
					}
					else if (party->SetChanneller(player_msg.SenderID(), false))
					{
						if (n != NO_ACTOR)
						{
							LoadString(hInstance, IDS_CHANNEL_CLOSED, disp_message, sizeof(disp_message));
							_stprintf(message, disp_message, n->Name());
							display->DisplayMessage(message);
						}
					}
					break;
                case RMsg_PlayerMsg::CHANNELKILL:
                    n = actors->LookUpNeighbor(player_msg.SenderID());
                    if(n != NO_ACTOR)
                    {
                        LoadString (hInstance, IDS_RECEIVE_CHANNELKILL, disp_message, sizeof(disp_message));
                        _stprintf(message, disp_message, n->Name());
                        display->DisplayMessage(message);
                    }
                    break;
				case RMsg_PlayerMsg::RANDOM:
					n = actors->LookUpNeighbor(player_msg.SenderID());
					cDS->PlaySound(LyraSound::RANDOM);
					if (player_msg.SenderID() == player->ID())
					{	// there's a random number in both state fields
						LoadString (hInstance, IDS_RANDOM_SELF, disp_message, sizeof(disp_message));
					_stprintf(message, disp_message, (int)player_msg.State1());
						display->DisplayMessage(message);
					} else if (n != NO_ACTOR)
					{
						LoadString (hInstance, IDS_RANDOM_OTHER, disp_message, sizeof(disp_message));
					_stprintf(message, disp_message, n->Name(), (int)player_msg.State1());
						display->DisplayMessage(message);
					}
					break;
				case RMsg_PlayerMsg::TRAP_NIGHTMARE:	 // skill, guild flags
					arts->ApplyTrapMare(player_msg.State1(), player_msg.State2(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::FINGER_OF_DEATH:	  // not used, not used
					arts->ApplyDie(player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::GRANT_XP:			  // units of 1000, units of 100
					arts->ApplyGrantXP(player_msg.State1(), player_msg.State2(), player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::GRANT_XP_NEGATIVE:			// units of 1000, units of 100
					arts->ApplyGrantXP(player_msg.State1()*-1, player_msg.State2()*-1, player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::TERMINATE:		  // not used, not used
					arts->ApplyTerminate(player_msg.SenderID());
					break;
				case RMsg_PlayerMsg::SUSPEND:		  // # days, not used
					arts->ApplySuspend(player_msg.State1(), player_msg.SenderID());
					break;

				//case RMsg_PlayerMsg::TRIGGER_SOUND:
				//	n = actors->LookUpNeighbor(player_msg.SenderID());
				//	if (n != NO_ACTOR)
				//	{ // trigger sound; some sounds may have special effects
				//		cDS->PlaySound(player_msg.State1(), n->x, n->y);
				//		if (player_msg.State1() == LyraSound::AGOKNIGHT_ROAR)
				//			arts->ApplyDeafen(3, player_msg.SenderID(), true);
				//	}
				//	break;

				case RMsg_PlayerMsg::ASCEND:
					arts->ResponseAscend(player_msg.State1(), player_msg.State2());
					break;

				case RMsg_PlayerMsg::TRAIN_SELF:
					arts->ResponseTrainSelf(player_msg.State1(), player_msg.State2());
					break;

				case RMsg_PlayerMsg::DEMOTE:
					arts->ApplyDemote(player_msg.State1());
					break;

				case RMsg_PlayerMsg::DEMOTE_ACK:
					arts->ResponseDemote(true, player_msg.SenderID(), player_msg.State1(), player_msg.State2());
					break;

				case RMsg_PlayerMsg::DEMOTE_FAIL:
					arts->ResponseDemote(false, player_msg.SenderID(), player_msg.State1(), player_msg.State2());
					break;

				case RMsg_PlayerMsg::NEWBIE_ENTERED:
#ifndef GAMEMASTER
					n = actors->LookUpNeighbor(player_msg.SenderID());
					if ((n != NO_ACTOR) && (!n->NewbieMessage()) && 
						(player->IsKnight() || player->IsRuler()) &&
						(!n->Avatar().Hidden()))
					{	// print message for knights only
						LoadString (hInstance, IDS_NEWBIE_ENTERED, disp_message, sizeof(disp_message));
						_stprintf(message, disp_message, n->Name());
						display->DisplayMessage(message);
						n->SetNewbieMessage(true);
					}
#endif
					break;

				case RMsg_PlayerMsg::SUMMON_PRIME:
					arts->ApplySummonPrime(player_msg.State1(), player_msg.State2());
					break;

				case RMsg_PlayerMsg::GRANT_PPOINT:
					player->SetPPoints(player->PPoints() + 1);
					LoadString (hInstance, IDS_GOT_PP, disp_message, sizeof(disp_message));
					display->DisplayMessage(disp_message);					
					break;

				case RMsg_PlayerMsg::UNKNOWN:

				default:
					break;
			}
		}
		break;

		case RMsg::CHANGEAVATAR:
		{
			RMsg_ChangeAvatar avatar_msg;
			if (avatar_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_AVATAR_CHANGE_MSG); return; }
			n = actors->LookUpNeighbor(avatar_msg.PlayerID());
			if (n != NO_ACTOR)
				n->SetAvatar(avatar_msg.Avatar());
		}
		break;

		case RMsg::RCVAVATARDESCRIPTION:
		{
			RMsg_RcvAvatarDescription avatar_msg;
			if (avatar_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_AVATAR_DESC_MSG); return; }
			n = actors->LookUpNeighbor(avatar_msg.PlayerID());
			if (n != NO_ACTOR)
			{
				TCHAR* descrip = (TCHAR*)(avatar_msg.Description());
				if (options.adult_filter)
					descrip = AdultFilter((TCHAR*)avatar_msg.Description());
				LoadString (hInstance, IDS_AVATAR_DESCRIP, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, n->Name(), descrip);
				display->DisplayMessage(message);
			}
		}
		break;

		case RMsg::ROOMDESCRIPTION:
		{
			RMsg_RoomDescription rmDesc_msg;
			if (rmDesc_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERR_READ_AVATAR_DESC_MSG); return; }
			if ((rmDesc_msg.LevelID() == level->ID()) &&
				(rmDesc_msg.RoomID() == player->Room()) &&
				(rmDesc_msg.Description() != _T("\0")))
			{
				TCHAR* rmDescrip = (TCHAR*)(rmDesc_msg.Description());
				_stprintf(message, "%s", rmDescrip);
				display->DisplayMessage(message, false);
			}
		}
		break;


		default:
			break;
	}
}


// We have received position updates from the server.
// Note that the info received here won't create or delete any actors;
// it only changes attributes of the actors for our neighbors.
void cGameServer::OnPositionUpdate(HWND hWindow, WPARAM wParam, LPARAM lParam)
{
	int				iRc, iAddrLen, buffer_size;
	sockaddr_in 		saddr_remote; // remote address structure
	RMsg_PlayerUpdate position_msg;
	char* 			buffer;
	LmMesgHdr			udpheader; // message header for lg message

	if ((wParam != sd_udp) || !sd_udp) // wrong socket
		return;

	if (WSAGETSELECTEVENT(lParam) == FD_READ)
	{
		if ((iRc = WSAGETSELECTERROR(lParam)) != 0)	// error
		{
			SOCKETS_ERROR(0);
			return;
		}

		iAddrLen = sizeof(saddr_remote);
		buffer_size = position_msg.MaxMessageSize() + HEADER_SIZE;
		buffer = new char[buffer_size];

		// read into a buffer, and pull out the header and
		iRc = recvfrom( sd_udp, (char*)buffer, buffer_size,
			0, (struct sockaddr *)&saddr_remote, &iAddrLen);
		
		//if (NTOHS(saddr_remote.sin_port) != level_server_addr.sin_port)
		//	return;

		if ((saddr_remote.sin_addr.s_addr != level_server_addr.sin_addr.s_addr))
			return;

		//_tprintf("ls: %d, %d incoming: %d, %d\n", level_server_addr.sin_port, level_server_addr.sin_addr.s_addr, NTOHS(saddr_remote.sin_port), saddr_remote.sin_addr.s_addr);

		if (iRc == SOCKET_ERROR)
		{
			if ((iRc=WSAGetLastError()) != WSAEWOULDBLOCK)
				SOCKETS_ERROR(iRc);
			return;
		}
		memcpy(udpheader.HeaderAddress(), buffer, HEADER_SIZE);
		udpheader.SetByteOrder(ByteOrder::NETWORK);
		LmMesgBuf udpbuf(udpheader.MessageSize());
		udpbuf.ReadHeader(udpheader);
		memcpy(udpbuf.MessageAddress(), (buffer + HEADER_SIZE), udpheader.MessageSize());

		delete [] buffer; buffer = NULL;

		// check that message is a position update message
		if (udpheader.MessageType() != RMsg::PLAYERUPDATE)
		{
			NONFATAL_ERROR(IDS_MSG_ERR);
			return;
		}

		if (position_msg.Read(udpbuf) < 0) { GAME_ERROR(IDS_ERR_READ_LOCAL_GRP_INFO_MSG); return; }

		num_packets_received++;

		//_tprintf("cp neighbors: %d msg neighbors: %d\n",cp->NumNeighbors(), position_msg.NumPlayers());
		//for (i=0; i<position_msg.NumPlayers(); i++)
			//_tprintf("index %d, id %d, local %d\n",i,position_msg.Update(i).playerid,position_msg.Update(i).local);
		if (!logged_into_level)
			return;
		if (!got_peer_updates)
		{
			  got_peer_updates = true;
		}

		this->HandlePositionUpdate(position_msg);
	}
	
//	this->CheckInvariants(__LINE__);
	
	return;
}

void cGameServer::HandlePositionUpdate(RMsg_PlayerUpdate& position_msg)
{
	if (!logged_into_level)
			return;
    if (!got_peer_updates)
    {
		  got_peer_updates = true;
    }

	cNeighbor			*n;
	int					i;
	float 				oldxheight;
	__int64				old_flags;


	//_tprintf("got update message with %d players\n", position_msg.NumPlayers());
	// Now loop through all our neighbors & update them
	for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
	{
		if (n->Avatar().Hidden())
			continue;

		n->MakeOutsider(); // default
		old_flags = n->flags;

		//_tprintf("got update; searching for name %s, id %d; x = %d, y= %d...\n",n->Name(),n->ID(),position_msg.PeerUpdate(i).X(), position_msg.PeerUpdate(i).Y());
		//_tprintf("got update; searching for name %s, id %d, rtID %d vs message rtID %d...\n",n->Name(),n->ID(), n->RealtimeID(), position_msg.PeerUpdate(i).RealtimeID());
		for (i=0; i<position_msg.NumPlayers(); i++)
		{
			//printf("comparing rtid/room %d,%d vs %d,%d\n", n->RealtimeID(), n->Room(), position_msg.PeerUpdate(i).RealtimeID(), player->Room());
			if (position_msg.PeerUpdate(i).RealtimeID() == n->RealtimeID())
			{
				// priorities for poses:
				// 1 - hit
				// 2 - attack
				// 3 - evoke
				// 4 - jump
				// 5 - wave
				// 6 - walk
				// 7 - stand

				n->SetUpdateFlags(position_msg.PeerUpdate(i));
				oldxheight =  level->Sectors[n->sector]->FloorHt(n->x,n->y)+level->Sectors[n->sector]->HtOffset+n->physht;

				if (position_msg.PeerUpdate(i).SoundID() != LyraSound::NONE)
					cDS->PlaySound(position_msg.PeerUpdate(i).SoundID(), n->x, n->y);

				if (position_msg.PeerUpdate(i).Local())
				{	// in real time update local groupo
					n->MakeLocal();
					//_tprintf("%s is local at %d! x = %d, y = %d\n",n->Name(), i, position_msg.PeerUpdate(i).X(), position_msg.PeerUpdate(i).Y());
					if (!(n->Moved())) // place only if we haven't moved them locally since last position_msg.PeerUpdate(i)...
						n->PlaceActor((float)position_msg.PeerUpdate(i).X(),(float)position_msg.PeerUpdate(i).Y(),0,position_msg.PeerUpdate(i).Angle(), SET_NONE, false);
					//_tprintf("new position at %d: %d, %d, %d\n",LyraTime(),(int)n->x,(int)n->y,(int)n->angle);
					
					n->SetStrafe(FALSE);
					if (position_msg.PeerUpdate(i).Flags() & LmPeerUpdate::LG_STRAFING)
					{
						n->SetStrafe(TRUE);
						n->velocity = MAXSTRAFE;
					}
					else if (position_msg.PeerUpdate(i).Flags() & LmPeerUpdate::LG_WALKING)
					{
						if (n->IsMonster())
							n->velocity = MAXWALK * SHAMBLE_SPEED;
						else
							n->velocity = MAXWALK;
					}
					else
						n->velocity = 0.0f;
					
					if (position_msg.PeerUpdate(i).Flags() & LmPeerUpdate::LG_RUNNING)
						n->velocity = n->velocity * RUN_SPEED;
					
					if (position_msg.PeerUpdate(i).Flags() & LmPeerUpdate::LG_BACKWARDS)
						n->velocity = -n->velocity;
				}
				else
				{	//_tprintf("%s is outsider at %d!\n",n->Name(), i);
					n->SetStrafe(FALSE);
					n->SetXHeight();
					n->destx = (float)position_msg.PeerUpdate(i).X();
					n->desty = (float)position_msg.PeerUpdate(i).Y();
					n->MakeOutsider();
				}
				
				n->angle = position_msg.PeerUpdate(i).Angle();
				//_stprintf(message, "Setting neighbor angle to %d", n->angle);
				//display->DisplayMessage(message, false);

				n->SetMoved(false);
				
				if (position_msg.PeerUpdate(i).Wave())
				{
					n->SetWaving(true);
					n->SetPose(WAVE);
				}
				else
					n->SetWaving(false);
				
				if (position_msg.PeerUpdate(i).Flags() & LmPeerUpdate::LG_JUMPED)
				{	// jumped!
					n->SetXHeight();
					n->vertforce = -40.0f;
					n->SetJumping(true);
					n->SetPose(JUMP);
				}
				else if (n->z == oldxheight)
					n->SetXHeight();
				
				if ((position_msg.PeerUpdate(i).Flags() & LmPeerUpdate::LG_SOULSPHERE) &&
					(!(old_flags & ACTOR_SOULSPHERE)))
					n->Dissolve();
				else if (!(position_msg.PeerUpdate(i).Flags() & LmPeerUpdate::LG_SOULSPHERE) &&
					(old_flags & ACTOR_SOULSPHERE))
					n->Form();
				
				if (position_msg.PeerUpdate(i).Flags() & LmPeerUpdate::LG_EVOKED)
				{
					if (!n->EvokedFX().Active())
					{
						n->EvokedFX().Activate(position_msg.PeerUpdate(i).Harmful(),
							position_msg.PeerUpdate(i).PrimaryColor(),
							position_msg.PeerUpdate(i).SecondaryColor(),
							false);
					}
				}
				else
					n->EvokedFX().DeActivate();
				
				if (position_msg.PeerUpdate(i).Flags() & LmPeerUpdate::LG_EVOKING)
				{
					if (!n->EvokingFX().Active())
					{
						n->EvokingFX().Activate(position_msg.PeerUpdate(i).Harmful(),
							position_msg.PeerUpdate(i).PrimaryColor(),
							position_msg.PeerUpdate(i).SecondaryColor(),
							true);
						n->SetEvoking(true);
						n->SetPose(EVOKING);
					}
				}
				else
				{
					n->EvokingFX().DeActivate();
					n->SetEvoking(false);
				}
				
				// Determine if a new death or attack has just happened
				//printf("Attack bits: %d Sanc: %d Now - Last Attack: %d Shot Interval: %d Wpn Bitmap: %d\n",
				//	position_msg.PeerUpdate(i).AttackBits(), level->Rooms[player->Room()].flags & ROOM_SANCTUARY,
				//	(LyraTime() - n->LastAttack()), SHOT_INTERVAL, position_msg.PeerUpdate(i).WeaponBitmap());
	
				if (position_msg.PeerUpdate(i).AttackBits() && !(level->Rooms[player->Room()].flags & ROOM_SANCTUARY) &&
					((LyraTime() - n->LastAttack()) > SHOT_INTERVAL) && position_msg.PeerUpdate(i).WeaponBitmap())
				{
					//_tprintf(_T("Got a new attack at time %d Last = %d!!!\n"),LyraTime(),n->LastAttack());
					n->SetLastAttack(LyraTime());
					int height_delta = position_msg.PeerUpdate(i).HeightDelta();
					if (height_delta > MAX_HEIGHT_DELTA)
						height_delta -= MAX_HEIGHT_DELTA*2;
					int velocity = position_msg.PeerUpdate(i).WeaponVelocity();
					
					cMissile *m = new cMissile(n, position_msg.PeerUpdate(i).WeaponBitmap(), n->angle,
						height_delta, velocity,  position_msg.PeerUpdate(i).WeaponEffect(),
						position_msg.PeerUpdate(i).WeaponDamage(), NULL, 0, n->sector);
					n->SetMissile(m);
					if (m->Melee())
						n->SetPose(MELEE_ATTACK);
					else
						n->SetPose(MISSILE_ATTACK);
					//_tprintf("Finished with new attack.\n");
				}
				
				unsigned int neighbor_hit_bits = position_msg.PeerUpdate(i).HitBits();
				// we walk through the hit_bits, examining the different positions
				// and comparing to the last hit time to determine if we should have
				// the neighbor react to this hit
				bool hit = false;
				// always react to a hit in the first position
				if (neighbor_hit_bits & 0x01)
					hit = true;
				// if there are hits in other positions, react if not within the delay
				neighbor_hit_bits = neighbor_hit_bits & 0xFE;
				if (neighbor_hit_bits && (LyraTime() - n->LastHit() > HIT_INTERVAL))
					hit = true;
				
				if (hit)
				{
					n->SetLastHit(LyraTime());
					Scream(n->Avatar().AvatarType(), n, false);
					n->SetPose(INJURED);
				}
				break;
			}
		}
	}
	actors->IterateNeighbors(DONE);

	
	return;
}


// Log into game server

void cGameServer::Login(int type)
{
	int			iRc,i;
	
	if (sd_game)
		this->Logout(GMsg_Logout::LOGOUT_NORMAL, false);
	
	login_type = type;
	
	header_bytes_read = body_bytes_read = 0;
	reading_header = TRUE;

#ifdef AGENT
	// find local IP address

#if 0 // this code is broken, and not really necessaryl
	char			szIPAddr[255];
	LPHOSTENT		lphp;

	iRc = gethostname( szIPAddr,255 );
	if (iRc == SOCKET_ERROR)
	{

		LoadString (hInstance, IDS_NETWORK_INIT_ERROR, disp_message, sizeof(disp_message));
		GAME_ERROR(disp_message);
		return;
	}
	lphp = gethostbyname( szIPAddr );
	if (lphp == NULL)
	{
		LoadString (hInstance, IDS_NETWORK_INIT_ERROR, disp_message, sizeof(disp_message));
		GAME_ERROR(disp_message);
		return;	
	}
#endif

	game_server_addr.sin_port = htons ( agent_gs_port );
	game_server_addr.sin_addr.s_addr = inet_addr ( agent_gs_ip_address );
//	localIP = *(struct in_addr *) (lphp->h_addr);

#else


#ifdef ROUND_ROBIN
	// go through the servers in round-robin fashion until we find
	// one that let's us log in
	unsigned short port_num = 0;
	int num_open_ports = DEFAULT_NUM_GAME_SERVERS - login_attempts;
	if (1 > num_open_ports)
	{
		if (game_full)
			LoadString (hInstance, IDS_LOGIN_GAMEFULL, disp_message, sizeof(disp_message));
		else
			LoadString (hInstance, IDS_GAME_UNAVAILABLE, disp_message, sizeof(disp_message));

		_tprintf(_T("Login Problem: %s\n"), disp_message);
		this->ServerError(disp_message);
		return;
	}
	int target = ((rand())%num_open_ports)+1; // pick a server randomly
	int num_open_ports_found = 0;
	for (i=0; i<DEFAULT_NUM_GAME_SERVERS; i++)
	{
		if (false == game_server_full[i])
		{
			num_open_ports_found++;
			if (num_open_ports_found == target)
			{
			if (options.debug) {
			//	if (1) {
					port_num = DEFAULT_DEBUG_SERVER_PORT;
					//options.debug = false;
				}
				else
					port_num = DEFAULT_GAME_SERVER_PORT + i;
				game_server_full[i] = true;
				break;
			}
		}
	}
	LPHOSTENT lphp = gethostbyname(options.game_server);
	game_server_addr.sin_addr.s_addr = ((struct in_addr far*)(lphp->h_addr_list[0]))->s_addr;
	//game_server_addr.sin_addr.s_addr = inet_addr (options.game_server );
	game_server_addr.sin_port=htons( port_num );
#ifdef UL_DEV
		LoadString (hInstance, IDS_TRYING_PORT, disp_message, sizeof(disp_message));
		_stprintf(message,disp_message,port_num);
		display->DisplayMessage(message, false);
#endif

#endif //  round robin code

#ifdef GAMED_POINTER
	// now figure out which port to connect the client to 
	// by pulling down a file via HTTP
	// with the IP address/port of the game server to use

	// download a text file via HTTP

	const int buffer_length = 64;
	char gamed_text[buffer_length];
	DWORD length;

	hInternet = InternetOpen("Underlight 2000 Launcher", NULL, NULL, NULL, 0); //INTERNET_FLAG_ASYNC); 
	if (!hInternet)
	{
		LoadString (hInstance, IDS_NO_HTTP1, disp_message, sizeof(disp_message));
		GAME_ERROR(disp_message);
		return;
	}
	
	hVersionFile = InternetOpenUrl(hInternet, options.gamed_URL, NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE, 1);
	if (!hVersionFile)
	{
		LoadString (hInstance, IDS_NO_HTTP2, disp_message, sizeof(disp_message));
		GAME_ERROR(disp_message);
		return;
	}

	//URL has been opened, initiate file read
	if (!InternetReadFile(hVersionFile, (LPVOID *)gamed_text, buffer_length, &length))
	{
		LoadString (hInstance, IDS_NO_HTTP3, disp_message, sizeof(disp_message));
		GAME_ERROR(disp_message);
		return;
	}
	
	InternetCloseHandle(hVersionFile);

	for (i=0; i<length; i++)
	{
		if (gamed_text[i] == '\n')
		{
			gamed_text[i] = '\0';
			game_server_addr.sin_addr.s_addr = inet_addr ( gamed_text );

			  // if we have an IP/port to use in options, use it 
			  // instead; either we specified it at login (for dev), or
			  // we got a GAMEFULL message telling us where to go next
			if (0 < _tcslen(options.game_server)) 
				game_server_addr.sin_addr.s_addr = inet_addr (options.game_server );

			char* port_text = (char*)(gamed_text + i + 1);
			if (options.server_port > 0) 
				game_server_addr.sin_port=htons( options.server_port );
			else 
			{
				unsigned short port_num = _ttoi(port_text);
				game_server_addr.sin_port=htons( port_num );
#if (defined(UL_DEBUG) || defined(ALT))
				LoadString (hInstance, IDS_PORTNUM, message, sizeof(message));
			 	_stprintf(disp_message, message, port_num);
				display->DisplayMessage(disp_message);
#endif

			}
			break;
		}
		
	}
#endif // GAMED_POINTER
#endif // AGENT


	// set up connection to game server
	sd_game = socket(PF_INET, SOCK_STREAM, 0);
	if (sd_game == INVALID_SOCKET)
	{
		SOCKETS_ERROR(0);
		return;
	}

	i = 1; // no delay for outgoing data
	iRc = setsockopt( sd_game, IPPROTO_TCP, TCP_NODELAY, (char *)&i, sizeof(i));
	if (iRc == SOCKET_ERROR)
	{
		SOCKETS_ERROR(0);
		return;
	}

	if (options.bind_local_tcp)
	{ // bind sockets to local port number
		struct sockaddr_in saddr;
		saddr.sin_family = PF_INET;
		saddr.sin_port=htons( options.bind_local_tcp );
		saddr.sin_addr.s_addr = htonl( INADDR_ANY );

		iRc = bind( sd_game, (struct sockaddr *) &saddr,
						 sizeof(saddr) );
		if (iRc == SOCKET_ERROR)
		{
			SOCKETS_ERROR(0);
			return;
		}
	}


	i = 1; // send keepalives...
	iRc = setsockopt( sd_game, SOL_SOCKET, SO_KEEPALIVE, (char *)&i, sizeof(i));
	if (iRc == SOCKET_ERROR)
	{
		SOCKETS_ERROR(0);
		return;
	}

	BOOL option = TRUE; // allow reuse of same address
	iRc = setsockopt( sd_game, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option));
	if (iRc == SOCKET_ERROR)
	{
		SOCKETS_ERROR(0);
		return;
	}

	// go async...
	iRc = WSAAsyncSelect( sd_game, cDD->Hwnd_Main(), WM_GAME_SERVER_DATA,
							FD_READ | FD_CLOSE | FD_CONNECT);
	if (iRc == SOCKET_ERROR)
	{
		SOCKETS_ERROR(0);
		return;
	}

	iRc = connect( sd_game, (struct sockaddr *) &game_server_addr,
					sizeof(game_server_addr) );
	// will yield a blocking error...
	if (iRc == SOCKET_ERROR)
		if (iRc=WSAGetLastError() != WSAEWOULDBLOCK)
			SOCKETS_ERROR(iRc);
#ifdef UL_DEBUG // This would be a good place to access before the MOTD
	LoadString (hInstance, IDS_DEVELOPMENT_BUILD, disp_message, sizeof(disp_message));
	display->DisplayMessage(disp_message);
#endif

	logging_in = true;

	return;
}

// Log into a new level
void cGameServer::LevelLogin(void)
{
	LmPeerUpdate	update;
	GMsg_GotoLevel gotolevel_msg;

	if (logged_into_level)
		this->LevelLogout(RMsg_Logout::LOGOUT);

	LoadString (hInstance, IDS_ENTER_LEVEL, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, level->Name(level->ID()));
	display->DisplayMessage(message, false);

	curr_level_id = level->ID();
	if (player->flags & ACTOR_FLY)
		player->RemoveTimedEffect(LyraEffect::PLAYER_FLYING);
	this->FillInPlayerPosition(&update);

	player->SetRoom(player->x, player->y);

	gotolevel_msg.Init(level->ID(), player->Room(), update);
	sendbuf.ReadMessage(gotolevel_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

	party = new cParty();

	SetLoggedIntoRoom(false);

	got_peer_updates = false;

	this->CheckInvariants(__LINE__);
	return;
}

// ask for room populations; used by agents to determine where to respawn
void cGameServer::FindRoomPopulations(lyra_id_t level_id)
{
	GMsg_GetLevelPlayers getplayers_msg;

	if (!logged_into_game)
	{
		LoadString (hInstance, IDS_AWAIT_CONNECTION, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		return;
	}

	getplayers_msg.Init(level->ID());
	sendbuf.ReadMessage(getplayers_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
	return;
}

void cGameServer::UpdateExpectedPackets(DWORD interval)
{
	int num_neighbors = 0;
	cNeighbor* n;

	//_stprintf(message, "Checking number of neighbors...player in room %d\n", player->Room());
	//LOGIT(message);

	for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
	{
		if ((n->Room() != player->Room()) || // in another room
			(n->ID() == player->ID())) // dummy
		{
			//_stprintf(message, "Player %s in room %d IGNORED\n", n->Name(), n->Room());
			//display->DisplayMessage(message);
			continue;
		}

		//_stprintf(message, "Player %s in room %d ADDED\n", n->Name(), n->Room());
		//LOGIT(message);
		num_neighbors++;
	}
	actors->IterateNeighbors(DONE);

	//_stprintf(message, "Total number of neighbors: %d\n", num_neighbors);
	//LOGIT(message);

	// we should be getting updates once every 160 ms from the server
	if ((num_neighbors > 0) && this->LoggedIntoLevel())
	{
		//_tprintf("nn: %d interval: %d expected: %d\n", num_neighbors, interval, num_packets_expected);
		num_packets_expected += (interval/160);
	}
}

// if we aren't getting enough udp packets, exit
void cGameServer::CheckUDPConnectivity(void)
{
	if ((num_packets_expected > 180) && this->LoggedIntoLevel() &&
		//(num_packets_received < (int)(num_packets_expected/10)))
		(num_packets_received == 0))
	{
		LoadString(hInstance, IDS_PACKETS_ERR, temp_message, sizeof(temp_message));
		_stprintf(errbuf, temp_message, num_packets_expected, num_packets_received);

		_tprintf(_T("%s"), errbuf);
		LoadString (hInstance, IDS_CRAP_NETWORKING, disp_message, sizeof(disp_message));
		GAME_ERROR(disp_message);
	}

	num_packets_expected = num_packets_received = 0;
	return;
}

// returns false & displays a message if we're not logged in
bool cGameServer::GetItem(cItem *item)
{
	GMsg_GetItem getitem_msg;

	if (!logged_into_game)
	{
		LoadString (hInstance, IDS_AWAIT_CONNECTION, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);

		return FALSE;
	}

	getitem_msg.Init(player->Room(), item->ID());
	sendbuf.ReadMessage(getitem_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

	item->SetStatus(ITEM_GETTING);

	return TRUE;
}

// returns false & displays a message if we're not logged in
bool cGameServer::CreateItem(cItem *item, int ttl, TCHAR *description)
{
	GMsg_CreateItem createitem_msg;

	if (!logged_into_game)
	{
		LoadString (hInstance, IDS_AWAIT_CONNECTION, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		return false;
	}
	if (creating_item)
	{
		LoadString (hInstance, IDS_ONE_ITEM_AT_A_TIME, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		return false;
	}
	creating_item = item;

	createitem_msg.Init(item->Lmitem());
	if (description != NULL)
		createitem_msg.SetDescription(description);

	sendbuf.ReadMessage(createitem_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

	item->SetStatus(ITEM_CREATING);

	return TRUE;

}

#ifdef GAMEMASTER

void cGameServer::ModifyItem(cItem *orig_item, TCHAR* new_name, int new_charges, int new_graphic, bool is_nopickup, bool is_artifact)
{
	cItem *new_item;
	LmItem info;
	LmItemHdr header;

	header.Init(0, 0, 0);
	header.SetFlags(orig_item->Lmitem().Header().Flags());
	header.SetGraphic(new_graphic);
	header.SetColor1(orig_item->Lmitem().Header().Color1());
	header.SetColor2(orig_item->Lmitem().Header().Color2());
	header.SetStateFormat(orig_item->Lmitem().Header().StateFormat());

	const void *state = orig_item->Lmitem().StateField(0);

	if (is_nopickup)
	{
		if (!orig_item->NoPickup())
			header.SetFlag(LyraItem::FLAG_NOPICKUP);
	}
	else {
		if (orig_item->NoPickup())
			header.ClearFlag(LyraItem::FLAG_NOPICKUP);
	}

	if (is_artifact)
	{
		// add noreap, if necessary
		if (!orig_item->NoReap())
			header.SetFlag(LyraItem::FLAG_NOREAP);

		// add always drop, if necessary
		if (!orig_item->AlwaysDrop())
			header.SetFlag(LyraItem::FLAG_ALWAYS_DROP);
	} else
	{
		// clear both noreap and always drop if the flags are set
		if (orig_item->AlwaysDrop())
			header.ClearFlag(LyraItem::FLAG_ALWAYS_DROP);

		if (orig_item->NoReap())
			header.ClearFlag(LyraItem::FLAG_NOREAP);
	}


	info.Init(header, new_name, 0, 0, 0);

	if (!CloneItemFunction(info, state, orig_item->ItemFunction(0)))
	{
		// fail out
		display->DisplayMessage("Cannot modify selected item function");
		return;
	}
	
	info.SetCharges(new_charges);

	new_item = new cItem(player->x, player->y, player->angle, info, ITEM_CREATING, 0,
		false, GMsg_PutItem::DEFAULT_TTL);

	if (orig_item->Lmitem().Flags() & LyraItem::FLAG_HASDESCRIPTION)
	{
		item_to_dupe = new_item;
		descript_callback = (&cGameServer::FinalizeItemModify);
		this->SendItemDescripRequest(orig_item->ID());
	}
	else
	{
		// just call the method directly
		this->FinalizeItemModify(new_item, NULL);
	}
}

void cGameServer::FinalizeItemModify(cItem *item_to_modify, TCHAR* description)
{
	// One more sanity check
	if ((cp->SelectedItem() == NO_ACTOR) || !(actors->ValidItem(cp->SelectedItem())))
		return;

	if (CreateItem(item_to_modify, GMsg_PutItem::DEFAULT_TTL, description))
	{
		// delete the existing item if we successfully create a new one
		cp->SelectedItem()->Destroy();
	}
}

void cGameServer::DuplicateItem(cItem *orig_item)
{
	// request the description if there is one
	if (orig_item->Lmitem().Flags() & LyraItem::FLAG_HASDESCRIPTION)
	{
		item_to_dupe = orig_item;
		descript_callback = (&cGameServer::FinalizeItemDuplicate);
		this->SendItemDescripRequest(orig_item->ID());
	}
	else 
	{
		// just finalize the duplication if there is no description
		this->FinalizeItemDuplicate(orig_item, NULL);
	}
}

void cGameServer::FinalizeItemDuplicate(cItem *orig_item, TCHAR* description)
{
	// One more sanity check
	if ((orig_item == NO_ACTOR) || !(actors->ValidItem(orig_item))) 				
		return; 

	cItem *new_item;
	LmItem info;
	LmItemHdr header;

	header.Init(0, 0, 0);
	header.SetFlags(orig_item->Lmitem().Header().Flags());
	header.SetGraphic(orig_item->Lmitem().Header().Graphic());
	header.SetColor1(orig_item->Lmitem().Header().Color1());
	header.SetColor2(orig_item->Lmitem().Header().Color2());
	header.SetStateFormat(orig_item->Lmitem().Header().StateFormat());

	const void *state = orig_item->Lmitem().StateField(0);
	info.Init(header, orig_item->Name(), 0, 0, 0);

	if (!CloneItemFunction(info, state, orig_item->ItemFunction(0)))
	{
		display->DisplayMessage("Cannot duplicate selected item function");
		return;
	}

	// set charges
	info.SetCharges(orig_item->Lmitem().Charges());
	int ttl = GMsg_PutItem::DEFAULT_TTL;
	
	new_item = new cItem(player->x, player->y, player->angle, info, ITEM_CREATING, 0,
		false, ttl);
	CreateItem(new_item, ttl, description);
}

bool cGameServer::CloneItemFunction(LmItem& info, const void *state, int item_function)
{
	// handle the item function
	switch (item_function)
	{
		case LyraItem::NOTHING_FUNCTION:
		{
			lyra_item_nothing_t do_nothing;
			memcpy(&do_nothing, state, sizeof(do_nothing));
			info.SetStateField(0, &do_nothing, sizeof(do_nothing));
			break;
		}
		case LyraItem::CHANGE_STAT_FUNCTION:
		{
			lyra_item_change_stat_t change_stat;
			memcpy(&change_stat, state, sizeof(lyra_item_change_stat_t));
			info.SetStateField(0, &change_stat, sizeof(change_stat));
			break;
		}
		case LyraItem::MISSILE_FUNCTION:
		{
			lyra_item_missile_t missile;
			memcpy(&missile, state, sizeof(lyra_item_missile_t));
			info.SetStateField(0, &missile, sizeof(missile));
			break;
		}
		case LyraItem::EFFECT_PLAYER_FUNCTION:
		{
			lyra_item_effect_player_t effect_player;
			memcpy(&effect_player, state, sizeof(lyra_item_effect_player_t));
			info.SetStateField(0, &effect_player, sizeof(effect_player));
			break;
		}
		case LyraItem::ARMOR_FUNCTION:
		{
			lyra_item_armor_t armor;
			memcpy(&armor, state, sizeof(lyra_item_armor_t));
			info.SetStateField(0, &armor, sizeof(armor));
			break;
		}
		case LyraItem::AMULET_FUNCTION:
		{
			lyra_item_amulet_t amulet;
			memcpy(&amulet, state, sizeof(lyra_item_amulet_t));
			info.SetStateField(0, &amulet, sizeof(amulet));
			break;
		}
		case LyraItem::ESSENCE_FUNCTION:
		{
			lyra_item_essence_t essence;
			memcpy(&essence, state, sizeof(lyra_item_essence_t));
			info.SetStateField(0, &essence, sizeof(essence));
			break;
		}
		case LyraItem::SUPPORT_FUNCTION:
		{
			lyra_item_support_t support;
			memcpy(&support, state, sizeof(lyra_item_support_t));
			info.SetStateField(0, &support, sizeof(support));
			break;
		}
		default:
		{			
			return false;
		}
		
	}

	return true;
}
#endif

// returns false & displays a message if we're not logged in
// drop item where the player is.
bool cGameServer::DropItem(cItem *item)
{
	GMsg_PutItem putitem_msg;
	GMsg_UpdateItem updateitem_msg;
	LmPosition pos;

	if (!logged_into_game || (!logged_into_level && !options.welcome_ai))
	{
		LoadString (hInstance, IDS_AWAIT_CONNECTION, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);

		return FALSE;
	}


	// check that there aren't already the max number of items in the room
	int items_in_room = 0;
	cItem *iterate_item;
	for (iterate_item = actors->IterateItems(INIT); iterate_item != NO_ACTOR; iterate_item = actors->IterateItems(NEXT))
		if ((iterate_item->Status() == ITEM_UNOWNED))
			items_in_room++;
	actors->IterateItems(DONE);

	if (items_in_room >= Lyra::MAX_ROOMITEMS)
	{
		LoadString (hInstance, IDS_MAX_ROOMITEMS, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);

		return FALSE;
	}


	pos.Init((short)item->x,(short)item->y,(short)item->z,(unsigned short)(item->angle));

	if ((item->Status() == ITEM_OWNED) && (item->NeedsUpdate()))
	{
		item->SetNeedsUpdate(false);
		updateitem_msg.Init(item->Lmitem());
		sendbuf.ReadMessage(updateitem_msg);
		send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
	}

	int ttl = item->ItemFunction(0) != LyraItem::PORTKEY_FUNCTION ? GMsg_PutItem::DEFAULT_TTL : 10 * (player->SkillSphere(Arts::PORTKEY) + 1);
	if (item->UseTTLForDrop())
		ttl = item->ExpireTime();
	putitem_msg.Init(player->Room(), item->ID(), pos, ttl);
	sendbuf.ReadMessage(putitem_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

	item->SetStatus(ITEM_DROPPING);

	return TRUE;
}

// returns false & displays a message if we're not logged in
// drop item where the player is.
bool cGameServer::DestroyItem(cItem *item)
{
	GMsg_DestroyItem destroyitem_msg;
	GMsg_DestroyRoomItem destroyroomitem_msg;

	if (!logged_into_game && !loading_inventory)
	{
		LoadString (hInstance, IDS_AWAIT_CONNECTION, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);

		return FALSE;
	}

	if (item->Status() == ITEM_OWNED)
	{	// destrying an item in inventory
		destroyitem_msg.Init(item->ID());
		sendbuf.ReadMessage(destroyitem_msg);
	}
	else if (item->Status() == ITEM_UNOWNED)
	{	// destroying an owned item
		destroyroomitem_msg.Init(player->Room(),item->ID());
		sendbuf.ReadMessage(destroyroomitem_msg);
	}
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

	item->SetStatus(ITEM_DESTROYING);

	return TRUE;
}

// returns false & displays a message if we're not logged in
// give away item where the player is.
bool cGameServer::GiveItem(cItem *item, cNeighbor *n)
{
	GMsg_GiveItem giveitem_msg;
	GMsg_UpdateItem updateitem_msg;

	if (!logged_into_game || !logged_into_level)
	{
		LoadString (hInstance, IDS_AWAIT_CONNECTION, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);

		return FALSE;
	}

	if (item->NeedsUpdate())
	{
		item->SetNeedsUpdate(false);
		updateitem_msg.Init(item->Lmitem());
		sendbuf.ReadMessage(updateitem_msg);
		send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
	}

	giveitem_msg.Init(n->ID(), item->ID());
	sendbuf.ReadMessage(giveitem_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

	item->SetStatus(ITEM_GIVING);

	return TRUE;
}

// returns false & displays a message if we're not logged in
bool cGameServer::ShowItem(cItem *item, cNeighbor *n)
{
	GMsg_ShowItem showitem_msg;

	if (!logged_into_game || !logged_into_level)
	{
		LoadString (hInstance, IDS_AWAIT_CONNECTION, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		return FALSE;
	}

	showitem_msg.Init(n->ID(), item->ID());
	sendbuf.ReadMessage(showitem_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

	return TRUE;
}


// returns false & displays a message if we're not logged in
// reponse to attempt to give item to player
bool cGameServer::TakeItemAck(int status, cItem *item)
{
	GMsg_TakeItemAck takeitemack_msg;

	if (!logged_into_game)
	{
		LoadString (hInstance, IDS_AWAIT_CONNECTION, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		return FALSE;
	}

	takeitemack_msg.Init(status, item->ID());
	sendbuf.ReadMessage(takeitemack_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

	if (status == GMsg_TakeItemAck::TAKE_YES)
		item->SetStatus(ITEM_RECEIVING);
	else
		item->SetTerminate();

	return TRUE;
}



void cGameServer::PostGoal(realmid_t goalid, int rank, int guild, int maxaccepted, int expirationdays,
		  int sugsphere, int sugstat, int guardian, TCHAR* summary, 
		  TCHAR* goaltext, TCHAR* keywords, unsigned int graphic, 
		  unsigned char charges, unsigned char color1, unsigned char color2,
		  unsigned char item_type, unsigned int field1, unsigned int field2, 
		  unsigned int field3, unsigned int quest_xp)
{
	if (!logged_into_game)
		return;

	//if (rank == Guild::QUEST) 
	//{
		//TCHAR sentence[Lyra::MAX_SPEECHLEN];
		//LoadString (hInstance, IDS_QUEST_REPORT, message, sizeof(message));
		//_stprintf(sentence, message, sugstat, sugsphere, summary, goaltext);
		//sentence[Lyra::MAX_SPEECHLEN-1] = '\0';
		//this->Talk(sentence, RMsg_Speech::REPORT_QUEST, Lyra::ID_UNKNOWN, true);
	//}

	GMsg_PostGoal postgoal_msg;
	postgoal_msg.Init(goalid, (short)rank, (short)guild, (short)maxaccepted,
		(short)expirationdays, (short)sugsphere, (short)sugstat, (short)guardian,
		summary, goaltext, 10, graphic,  charges,  color1,  color2,
		 item_type,  field1,  field2,  field3, quest_xp, keywords);
	
	sendbuf.ReadMessage(postgoal_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
}


void cGameServer::PostReport(realmid_t goal_id, int awardxp, TCHAR* recipient,
		  TCHAR* summary, TCHAR *report)
{
	if (!logged_into_game)
		return;

	GMsg_PostReport postreport_msg;
	postreport_msg.Init(goal_id, awardxp, recipient,
		summary, report);
	sendbuf.ReadMessage(postreport_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
}


void cGameServer::RequestGoalbook(void)
{
	if (!logged_into_game)
		return;

	GMsg_Goal getgoalbook_msg;
	getgoalbook_msg.Init(0, GMsg_Goal::GET_GOALBOOK_HEADERS);
	sendbuf.ReadMessage(getgoalbook_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
}


void cGameServer::RequestGoalHeaders(int sessionid, int guild, int rank, realmid_t lastseen)
{
	if (!logged_into_game)
		return;

	GMsg_GetGoalHdrs getgoalhdrs_msg;
	getgoalhdrs_msg.Init(sessionid, (short)guild, (short)rank, lastseen);
	sendbuf.ReadMessage(getgoalhdrs_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
}


void cGameServer::RequestGoalText(realmid_t goal)
{
	if (!logged_into_game)
		return;

	GMsg_Goal getgoaltext_msg;
	getgoaltext_msg.Init(goal, GMsg_Goal::GET_GOAL_TEXT);
	sendbuf.ReadMessage(getgoaltext_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
}


void cGameServer::RequestGoalDetails(realmid_t goal)
{
	if (!logged_into_game)
		return;

	GMsg_Goal getgoaldetails_msg;
	printf("requesting goal details!\n");
	getgoaldetails_msg.Init(goal, GMsg_Goal::GET_GOAL_DETAILS);
	sendbuf.ReadMessage(getgoaldetails_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
}


void cGameServer::RequestGoalGuardianFlag(realmid_t goal)
{
	if (!logged_into_game)
		return;

	GMsg_Goal getguardianflag_msg;
	getguardianflag_msg.Init(goal, GMsg_Goal::GET_GUARDIAN_FLAG);
	sendbuf.ReadMessage(getguardianflag_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
}


void cGameServer::RequestReportText(realmid_t report)
{
	if (!logged_into_game)
		return;

	GMsg_Goal getreporttext_msg;
	getreporttext_msg.Init(report, GMsg_Goal::GET_REPORT_TEXT);
	sendbuf.ReadMessage(getreporttext_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
}


void cGameServer::RequestReportHeaders(realmid_t goal, int guild, int rank, int sessionid, realmid_t lastseen)
{
	if (!logged_into_game)
		return;

	if (goal == Lyra::ID_UNKNOWN)
		goals->InitReportHeaders();

	GMsg_GetReportHdrs getreporthdrs_msg;
	getreporthdrs_msg.Init(goal, guild, rank, sessionid, lastseen);
	sendbuf.ReadMessage(getreporthdrs_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
}


void cGameServer::AcceptGoal(realmid_t goal)
{
	if (!logged_into_game)
		return;

	GMsg_Goal acceptgoal_msg;
	acceptgoal_msg.Init(goal, GMsg_Goal::ACCEPT_GOAL);
	sendbuf.ReadMessage(acceptgoal_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
}


void cGameServer::DeleteGoal(realmid_t goal)
{
	if (!logged_into_game)
		return;

	GMsg_Goal deletegoal_msg;
	deletegoal_msg.Init(goal, GMsg_Goal::EXPIRE_GOAL);
	sendbuf.ReadMessage(deletegoal_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
}


void cGameServer::DeleteReport(realmid_t report)
{
	if (!logged_into_game)
		return;

	GMsg_Goal deletereport_msg;
	deletereport_msg.Init(report, GMsg_Goal::DELETE_REPORT);
	sendbuf.ReadMessage(deletereport_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

}

// completing a goal means it is marked finished; no one else can accept it
void cGameServer::CompleteGoal(realmid_t goal)
{
	if (!logged_into_game)
		return;

	GMsg_Goal completegoal_msg;
	completegoal_msg.Init(goal, GMsg_Goal::COMPLETE_GOAL);
	sendbuf.ReadMessage(completegoal_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
}

// completing a quest means a dreamer who accepted it believes he has finished it
void cGameServer::CompleteQuest(lyra_id_t quest)
{
	if (!logged_into_game)
		return;

	GMsg_Goal completequest_msg;
	completequest_msg.Init(quest, GMsg_Goal::COMPLETE_QUEST);
	sendbuf.ReadMessage(completequest_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
	printf("Sending complete quest message!\n");
}


// completing a codex quest means a dreamer who accepted it believes he has finished it
// and has a codex in inventory with the right keywords to finish the quest
void cGameServer::IsCodexQuestCompleted(lyra_id_t quest)
{
	if (!logged_into_game)
		return;
	
	GMsg_Goal completequest_msg;
	completequest_msg.Init(quest, GMsg_Goal::DOES_HAVE_CODEX);
	sendbuf.ReadMessage(completequest_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
}
 
void cGameServer::RemoveFromGoalbook(realmid_t goal)
{
	if (!logged_into_game)
		return;

	GMsg_Goal removegoalbook_msg;
	removegoalbook_msg.Init(goal, GMsg_Goal::REMOVE_GOAL);
	sendbuf.ReadMessage(removegoalbook_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
}


void cGameServer::VoteGoal(realmid_t goal, int vote)
{
	if (!logged_into_game)
		return;

	GMsg_Goal votegoal_msg;
	if (vote == Guild::NO_VOTE)
		votegoal_msg.Init(goal, GMsg_Goal::VOTE_NO);
	else
		votegoal_msg.Init(goal, GMsg_Goal::VOTE_YES);
	sendbuf.ReadMessage(votegoal_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
}



// This function is called by the movement code whenever we switch
// between rooms.

void cGameServer::OnRoomChange(short last_x, short last_y)
{
	RMsg_GotoRoom		gotoroom_msg;
	LmPeerUpdate		update;
	cNeighbor			*n;
	cItem 			*item;

	if (!logged_into_level)
		return;

	if ((level->ID() == last_level_target) &&
		(player->Room() == last_room_target))
		return; // don't send duplicate messages


	this->FillInPlayerPosition(&update);

	// Now kill our neighbors. Locked neighbors won't be deleted.
	for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
	{
		if (n->Locked())
			n->SetRoom(0);
		else
			n->SetTerminate();
	}
	actors->IterateNeighbors(DONE);

	// Now kill the unowned/getting items
	for (item = actors->IterateItems(INIT); item != NO_ITEM; item = actors->IterateItems(NEXT))
		if (item->Status() == ITEM_UNOWNED)
			item->SetTerminate();
	actors->IterateItems(DONE);

	gotoroom_msg.Init(player->Room(), update, last_x, last_y);
	sendbuf.ReadMessage(gotoroom_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

	if ((player->IsUninitiated() && (level->ID() == RECRUITING_LEVEL_ID)))
		gs->SendPlayerMessage(0, RMsg_PlayerMsg::NEWBIE_ENTERED, 0, 0);
	if (player->flags & ACTOR_FLY)
		player->RemoveTimedEffect(LyraEffect::PLAYER_FLYING);
	LmAvatar avatar = player->Avatar();
	int value;
	if ((avatar.AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE) &&
		(level->Rooms[player->Room()].flags & ROOM_SANCTUARY))
	{ // player mares and dark mares get thrashed in sanct
		value = (avatar.AvatarType() -Avatars::MIN_NIGHTMARE_TYPE + 1)*-5;
		player->SetCurrStat(Stats::DREAMSOUL, value, SET_RELATIVE, player->ID() );
		LoadString (hInstance, IDS_SANCTUARY_HURTS, message, sizeof(message));
		display->DisplayMessage(message);
	}
	
	SetLoggedIntoRoom(false);
	room_change_time = LyraTime();
	got_peer_updates = false;

	last_level_target = level->ID();
	last_room_target = player->Room();

	this->CheckInvariants(__LINE__);
	return;
}



// send updates for stats and/or items, as needed
void cGameServer::UpdateServer(void)
{
	GMsg_UpdateItem updateitem_msg;
	GMsg_UpdateStats updatestats_msg;
	GMsg_ChangeStat changestat_msg;
	GMsg_Ping ping_msg;
	LmItem lmitem;
	LmStats stats;
	LmArts skills;
	cItem *item;
	int i;

	if (!connected_to_gs)
		return;

	if (!logged_into_game	|| 
		 options.welcome_ai || 
		 goals->Active()    ||
		 quests->Active())
	{ // 0 ping means don't display response
#ifndef AGENT
		ping_msg.Init(0, GMsg_Ping::PING_GAME_THREAD);
		sendbuf.ReadMessage(ping_msg);
		send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
#endif
		return;
	}

	 // send a changestat message with only needed changes
	int num_changes = 0;
	changestat_msg.Init(num_changes);

	for (i=0; i<NUM_PLAYER_STATS; i++)
	{
		if (player->CurrStatNeedsUpdate(i))
		{
			changestat_msg.SetNumChanges(num_changes+1);
			changestat_msg.SetRequestType(num_changes, GMsg_ChangeStat::SET_STAT_CURR);
			changestat_msg.SetStat(num_changes, i);
			changestat_msg.SetValue(num_changes, player->CurrStat(i));
			player->SetCurrStatNeedsUpdate(i, false);
			num_changes++;
		}
	}

	for (i=0; i<NUM_ARTS; i++)
		if (player->SkillNeedsUpdate(i))
		{
			changestat_msg.SetNumChanges(num_changes+1);
			changestat_msg.SetRequestType(num_changes, GMsg_ChangeStat::SET_SKILL);
			changestat_msg.SetStat(num_changes, i);
			changestat_msg.SetValue(num_changes, player->Skill(i));
			num_changes++;
			player->SetSkillNeedsUpdate(i, false);
		}

	changestat_msg.SetNumChanges(num_changes);

	// send 0 stat update if no changes to avoid timeout on server
	sendbuf.ReadMessage(changestat_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

	// update item state, if needed
	for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
  {
		if ((item->Status() == ITEM_OWNED) && (item->NeedsUpdate())
			&& !(item->Temporary()))
		{
			item->SetNeedsUpdate(false);
		//	LmItemHdr temp = item->ID();
		//	temp.ClearFlag(LyraItem::FLAG_IMMUTABLE);
			updateitem_msg.Init(item->Lmitem());
			sendbuf.ReadMessage(updateitem_msg);
			send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
		}
  }	
  actors->IterateItems(DONE);

	return;
}



// For when the player says something.
void cGameServer::Talk(TCHAR *talk, int speechType, lyra_id_t target, bool echo, bool allow_babble)
{
	RMsg_Speech speech_msg;

	if (logged_into_game)
	{ // just echo locally if we're in training, etc.
		speech_msg.Init(speechType, target, 0, talk);
		if (allow_babble && options.babble_filter)
			speech_msg.SetBabble(1);
		sendbuf.ReadMessage(speech_msg);
		send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

		if (echo && (speechType == RMsg_Speech::REPORT_BUG))
		{
			LoadString (hInstance, IDS_BUG_REPORT, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
		}
		else if (echo && (speechType == RMsg_Speech::REPORT_CHEAT))
		{
			LoadString (hInstance, IDS_CHEAT_REPORT, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
		}
		else if (echo && (speechType == RMsg_Speech::RP))
		{
			LoadString (hInstance, IDS_RP_REPORT, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
		}
	}

	switch (speechType)
	{
		case RMsg_Speech::WHISPER_EMOTE:
			break;
		case RMsg_Speech::EMOTE:
			display->DisplaySpeech(talk, player->Name(), speechType, true);
			break;
		case RMsg_Speech::RAW_EMOTE:
			display->DisplaySpeech(talk, _T(""), speechType, true);
			break;

		case RMsg_Speech::GLOBALSHOUT:
		case RMsg_Speech::SHOUT:
			LoadString (hInstance, IDS_PLAYER_SHOUT, disp_message, sizeof(disp_message));
			display->DisplaySpeech(talk, disp_message, speechType, true);
			break;
		case RMsg_Speech::WHISPER:
		{
			LoadString (hInstance, IDS_PLAYER_WHISPER, disp_message, sizeof(disp_message));
			cNeighbor *n = arts->LookUpNeighbor(target);
			if (n != NO_ACTOR)
			_stprintf(message, disp_message, n->Name());
			else if (target == player->ID())
				LoadString (hInstance, IDS_SELF, message, sizeof(message));
			else
				LoadString (hInstance, IDS_UNKNOWN, message, sizeof(message));
			display->DisplaySpeech(talk, message, speechType, true);
			break;
		}
		case RMsg_Speech::SPEECH:
			LoadString (hInstance, IDS_PLAYER_SPEAK, disp_message, sizeof(disp_message));
			display->DisplaySpeech(talk, disp_message, speechType, true);
		default:
			return;// no msg sent, no sound;
	}
	cDS->PlaySound(LyraSound::MESSAGE_SENT);

	return;
}



// Send out a join party request.
void cGameServer::JoinParty(realmid_t playerID, bool auto_join)
{
	RMsg_Party party_msg;
	int how =  auto_join ? RMsg_Party::JOIN_AUTO : RMsg_Party::JOIN_NORMAL;
	party_msg.Init(RMsg_Party::JOIN, playerID, how );
	sendbuf.ReadMessage(party_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

	return;
}

// Leave the current party.
void cGameServer::LeaveParty(realmid_t playerID)
{
	RMsg_Party party_msg;

	party_msg.Init(RMsg_Party::LEAVE, playerID, RMsg_Party::RC_UNUSED);
	sendbuf.ReadMessage(party_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

	return;
}

// Someone has asked to join our party; reject them!
void cGameServer::RejectPartyQuery(int reason, realmid_t playerID)
{
	RMsg_Party party_msg;

	// send reject message
	party_msg.Init(RMsg_Party::REJECT, playerID, reason);
	sendbuf.ReadMessage(party_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

	return;
}

// Someone has asked to join our party; accept them!
void cGameServer::AcceptPartyQuery(realmid_t playerID)
{
	RMsg_Party party_msg;

	// send reject message
	party_msg.Init(RMsg_Party::ACCEPT, playerID, RMsg_Party::RC_UNUSED);
	sendbuf.ReadMessage(party_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

	return;
}

// Send a message to another player
void cGameServer::SendPlayerMessage(lyra_id_t destination_id, short msg_type, short param1, short param2, short param3)
{
	RMsg_PlayerMsg player_msg;
	cNeighbor * n;

	// don't send an art msg to another player if you haven't received updates from them yet.
	// **** ALTERNATIVE TO WHO LIST DELAY ***
	if ((RMsg_PlayerMsg::ArtType(msg_type) != Arts::NONE) && (destination_id > 0)) {
		for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
		{
			if ((n->ID() == destination_id) && (!n->GotUpdate()))
			{
				actors->IterateNeighbors(DONE);
				return;
			}
		}
		actors->IterateNeighbors(DONE);
	}

	player_msg.Init(player->ID(), destination_id, msg_type, param1, param2, param3);
	sendbuf.ReadMessage(player_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
	return;
}

// avatar has changed; could be temp or permanent
void cGameServer::AvatarChange(LmAvatar new_avatar, bool permanent)
{
	GMsg_ChangeAvatar avatar_msg;

	if (permanent)
		avatar_msg.Init(new_avatar,GMsg_ChangeAvatar::AVATAR_PERMANENT);
	else
		avatar_msg.Init(new_avatar,GMsg_ChangeAvatar::AVATAR_CURRENT);
	sendbuf.ReadMessage(avatar_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

	return;

}

// send a locate avatar message
void cGameServer::LocateAvatar(GMsg_LocateAvatar& locate_msg)
{
	sendbuf.ReadMessage(locate_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
	return;
}


void cGameServer::FillInPlayerPosition(LmPeerUpdate *update, int trigger)
{
	//update->SetPlayerID(player->ID());
	update->SetRealtimeID(0);
	update->SetPosition((int)player->x, (int)player->y, (int) player->z);
	//_tprintf("set position at %d, %d at time %d\n",(int)player->x, (int)player->y, LyraTime());

	update->SetAngle(player->angle);
	//_stprintf(message, "Setting player angle to %d", player->angle);
	//display->DisplayMessage(message, false);
	update->SetFlags(0);
	update->SetSoundID(last_sound);

	last_sound = LyraSound::NONE;

	cArtFX &evoking = player->EvokingFX();
	cArtFX &evoked = player->EvokedFX();

	if (jumped)
		update->SetFlags(update->Flags() | LmPeerUpdate::LG_JUMPED);

	if (player->Waving())
		update->SetWave(1);
	else
		update->SetWave(0);

	if (evoked.Active())
	{
		update->SetFlags(update->Flags() | LmPeerUpdate::LG_EVOKED);
		if (!evoking.Active() || !(last_update.Flags() & LmPeerUpdate::LG_EVOKED))
		{
			update->SetHarmful(evoked.Harmful());
			update->SetPrimaryColor(evoked.MainColor());
			update->SetSecondaryColor(evoked.SecondColor());
		}
	}

	if (evoking.Active())
	{
		update->SetFlags(update->Flags() | LmPeerUpdate::LG_EVOKING);
		if (!evoked.Active() || !(last_update.Flags() & LmPeerUpdate::LG_EVOKING))
		{
			update->SetHarmful(evoking.Harmful());
			update->SetPrimaryColor(evoking.MainColor());
			update->SetSecondaryColor(evoking.SecondColor());
		}
	}


	hit_bits = hit_bits << 1;
	if (player->Injured())
		hit_bits = hit_bits | 0x01;
	update->SetHitBits(hit_bits);

	player->SetInjured(false);

	// this all works with agents because: their velocity is MAXWALK,
	// and on the receiving end speed is always set to SHAMBLE_SPEED
	// for monsters.

	if ((player->velocity < 0.0f) || (player->Strafe() == STRAFE_RIGHT))
		update->SetFlags(update->Flags() | LmPeerUpdate::LG_BACKWARDS);
	if ((player->velocity >= MAXWALK) || (player->velocity <= -MAXWALK))
		update->SetFlags(update->Flags() | LmPeerUpdate::LG_WALKING);
	else if (player->Strafe() != NO_STRAFE)
		update->SetFlags(update->Flags() | LmPeerUpdate::LG_STRAFING);
	if (player->Speed() == RUN_SPEED)
		update->SetFlags(update->Flags() | LmPeerUpdate::LG_RUNNING);

	if ((player->flags & ACTOR_INVISIBLE) || (player->flags & ACTOR_CHAMELED))
		update->SetFlags(update->Flags() | LmPeerUpdate::LG_INVISIBLE);

	if (player->flags & ACTOR_SOULSPHERE)
		update->SetFlags(update->Flags() | LmPeerUpdate::LG_SOULSPHERE);

	if (player->flags & ACTOR_FLY) 
		update->SetFlying(TRUE);

	attack_bits = attack_bits << 1;
	if (last_attack.time > last_peer_update)
		attack_bits = attack_bits | 0x01;
	update->SetAttackBits(attack_bits);
	if (trigger == TRIGGER_DEATH) {
		attack_bits = hit_bits = 0;
	}
	else
	{
		//if (last_attack.bitmap)
			//printf("last att bitmap: %d\n", last_attack.bitmap);
		update->SetWeaponBitmap(last_attack.bitmap);
		//if (last_attack.bitmap)
			//printf("weapon bitmap1: %d\n", update->WeaponBitmap());
		update->SetWeaponDamage(last_attack.damage);
		//if (last_attack.bitmap)
			//printf("weapon bitmap2: %d\n", update->WeaponBitmap());
		update->SetWeaponEffect(last_attack.effect);
		//if (last_attack.bitmap)
			///printf("weapon bitmap3: %d\n", update->WeaponBitmap());

		int height_delta = last_attack.height_delta;
		if (height_delta < 0)
			height_delta += MAX_HEIGHT_DELTA*2;
		int velocity = last_attack.velocity;
//		if (velocity < 0)
//			velocity += MAX_NETWORK_VELOCITY*2;
		//if ((player->Avatar().AvatarType() == Avatars::HORRON) &&	(attack_bits == 0x01))	{
			//_stprintf(message, "before: %d after: %d", last_attack.velocity, velocity);
			//MessageBox(NULL, "Velocity", message, MB_OK);	}

		update->SetWeaponVelocity(velocity);
		update->SetHeightDelta(height_delta);
		//update->SetSkill(last_attack.skill);
	} // these fields are ignored if no attack bit is set

	last_update = *update;

	return;
}

// Send out updated position info to other members of the local group
void cGameServer::SendPositionUpdate (int trigger)
{
	RMsg_Update position_msg;
	LmPeerUpdate update;
	sockaddr_in saddr;
	int iRc;

	if (!logged_into_level || !logged_into_room)
		return;

	//_tprintf("sending position update for reason %d\n",trigger);

	if (trigger != TRIGGER_DEATH) // always send on death...
	{
		if (trigger == TRIGGER_TIMER) // timer trigger
		{
			if (preupdate)
			{	// we already sent the update, so reset preupdate and exit
				preupdate = false;
// 		_tprintf("got timer, but already pretriggered...\n");
				return;
			}
			else
				preupdate = false;
		} else
		{
			if (preupdate)
				return; // not on timer, return if preupdated
			else
				preupdate = true;
		}
	}
///_tprintf("sending position update!\n");

	this->FillInPlayerPosition(&update,trigger);

///_tprintf("attack after: %d\n",attack);

	position_msg.Init(player->ID(), update);
	sendbuf.ReadMessage(position_msg);

	if (options.tcp_only) 
		send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
	else // send UDP position update message
	{	
		saddr.sin_family = PF_INET;
		saddr.sin_port=htons( portNumber );
		saddr.sin_addr.s_addr = game_server_addr.sin_addr.s_addr;
		iRc = sendto( sd_udp, (char *)sendbuf.BufferAddress(), sendbuf.BufferSize(), 0,
			(struct sockaddr *)&(saddr), sizeof(saddr));
	
		if (iRc == SOCKET_ERROR)
		{
			if ((iRc=WSAGetLastError()) != WSAEWOULDBLOCK)
				SOCKETS_ERROR(iRc);
			return;
		}
	}

	num_packets_out++;
	last_peer_update = LyraTime();
	jumped = FALSE;

	return;
}

bool cGameServer::CanUseItem(void)
{
	if ((LyraTime()-last_item_use_time)<ITEM_USE_INTERVAL)
	{
		cDS->PlaySound(LyraSound::MESSAGE);
    // display only once per session, but -always- play the sound.
		if (!displayed_item_use_message) {
			displayed_item_use_message = true;
			LoadString (hInstance, IDS_NO_ITEM_USE_YET, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message);
		}
		return FALSE;
	}
	last_item_use_time = LyraTime();
	return true;
}


// Launch an attack; if we're the player and the interval between now and the last shot
// isn't sufficiently long, or if we're in a no attack zone, return false.
// Otherwise, launch the attack and return TRUE.
// Item is an optional parameter specifying the item used to cause the attack.

bool cGameServer::PlayerAttack(int bitmap_id, int velocity,
								int effect, int damage_type, cItem *item, int art_id)
{
	if (level->Rooms[player->Room()].flags & ROOM_SANCTUARY)
	{ // no attack area
		LoadString (hInstance, IDS_ENTRY_NOFIGHTING, disp_message, 256);
		display->DisplayMessage (disp_message);
		return FALSE;
	}
	else if ((LyraTime()-last_attack.time)<SHOT_INTERVAL)
	{
		cDS->PlaySound(LyraSound::MESSAGE);
		return FALSE;
	}
	else if (!LegalMissilePosition(player->x, player->y, player->angle, player->sector))
	{
		LoadString (hInstance, IDS_NOROOM, disp_message, 256);
		display->DisplayMessage (disp_message);
		return FALSE;
	} 
#ifndef AGENT
#if 0 // remove BMP
	else if (this->LoggedIntoLevel() && !got_peer_updates)
	{
		LoadString (hInstance, IDS_AWAIT_UPDATE, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		return FALSE;
	}
#endif
#endif

	// player can launch an attack
	last_attack.time = LyraTime();
	last_attack.height_delta = player->HeightDelta();
	last_attack.bitmap = bitmap_id;
	last_attack.damage = damage_type;
	last_attack.effect = effect;
	last_attack.velocity = velocity;
	this->SendPositionUpdate(TRIGGER_ATTACK); // presend on attack
	cMissile *m = new cMissile(player, bitmap_id, player->angle,  player->HeightDelta(), velocity,
				 effect, damage_type, item, 0, player->sector, art_id);
	player->SetMissile(m);
	if (m->Melee() && !m->Invisible())
		player->SetUsingBlade(true);
	return TRUE;
}

void cGameServer::PingServer(void)
{
	GMsg_Ping ping_msg;

	if (!connected_to_gs)
		return;

	// for now, use nonce 1, do 1 at a time; nonce 0 = don't report
	ping_msg.Init(1);
	sendbuf.ReadMessage(ping_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
	ping_time = LyraTime();
}

bool cGameServer::AllowRightClick(void)
{
	return item_to_dupe == NULL;
}

void cGameServer::SendItemDescripRequest(LmItemHdr& itemheader)
{
	GMsg_GetItemDescription descrip_msg;

	// for now, use nonce 1, do 1 at a time; nonce 0 = don't report
	descrip_msg.Init(itemheader);
	sendbuf.ReadMessage(descrip_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
	return;
}

void cGameServer::UsePPoints(short how, short var1, short var2, short var3)
{
	GMsg_UsePPoint msg;
	msg.Init(how, var1, var2, var3);
	sendbuf.ReadMessage(msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
	return;
}


void cGameServer::GrantPPoint(lyra_id_t target_id, TCHAR* why)
{
	GMsg_GrantPPoint msg;
	msg.Init(target_id, why);
	sendbuf.ReadMessage(msg);
	player->AddGrantingPP(); // add to count
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
	return;
}


void cGameServer::SendAvatarDescrip(void)
{
	GMsg_SetAvatarDescription descrip_msg;

	// for now, use nonce 1, do 1 at a time; nonce 0 = don't report
	descrip_msg.Init(avatar_descrip);
	sendbuf.ReadMessage(descrip_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
	return;
}

void cGameServer::GetAvatarDescrip(lyra_id_t player_id)
{
	RMsg_GetAvatarDescription descrip_msg;

	descrip_msg.Init(player_id);
	sendbuf.ReadMessage(descrip_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
	return;
}

void cGameServer::GetRoomDescrip(int levelid, int roomid)
{
	RMsg_GetRoomDescription rmDesc_msg;

	rmDesc_msg.Init((short)levelid, (short)roomid);
	sendbuf.ReadMessage(rmDesc_msg);
	send(sd_game, (char *)sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
	return;
}

// Send the login message to the server upon completion of the welcome AI

void cGameServer::WelcomeAIComplete(void)
{ // UL3D!!!
	GMsg_PreLogin prelogin_msg;
	prelogin_msg.Init(Lyra::GAME_VERSION);
	sendbuf.ReadMessage(prelogin_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
}

// Log out of the current level. Could be due to a level change,
// leaving the game, or death

void cGameServer::LevelLogout(int how)
{
	cItem *item;
	cNeighbor *n;
	RMsg_Logout level_logout_msg;

	if (!logged_into_level)
		return;

	LoadString (hInstance, IDS_LEAVE_LEVEL, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, level->Name(curr_level_id));
	display->DisplayMessage(message, false);

	if (logged_into_game)
		this->UpdateServer();

	if ((how == RMsg_Logout::DEATH) && logged_into_level)
	{
		attack_bits = 0; // no attacks on death update
		hit_bits = 0;
		this->SendPositionUpdate(TRIGGER_DEATH); // say death trigger to always send
	}

	// delete all neighbors
	for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
		n->SetTerminate();
	actors->IterateNeighbors(DONE);

	for (item = actors->IterateItems(INIT); item != NO_ITEM; item = actors->IterateItems(NEXT))
	{
		if (item->Status() == ITEM_UNOWNED)
			item->SetTerminate();	// Now kill the items on the ground
	}

	actors->IterateItems(DONE);

	if (party)
	{
		if (party->RequestOutstanding() != Lyra::ID_UNKNOWN)
			party->RejectRequest();
		party->DissolveParty(false);
		delete party; party = NULL;
	}

	// Reset newly_alerts array table
	alert_count = 0;
	for (int i = 0; i < ALERT_TABLE_SIZE; i++) {
		_tcscpy(newly_alert[i].playerName, _T("0"));
		newly_alert[i].alertTime = NULL;
	}

	
	if (player->NeedItemFlagsOrSortingUpdate())
	{ // item flags or sorting has changed - save...
		cItem* (inventory[Lyra::INVENTORY_MAX]);
		int num_owned_items = SortInventory((cItem**)&inventory);
		level_logout_msg.Init(how, num_owned_items);

		for (int i=0; i<num_owned_items; i++)
			level_logout_msg.SetItem(i, inventory[i]->Lmitem().Header().Serial(), i+1, (unsigned char)(inventory[i]->InventoryFlags()));
	} else
		level_logout_msg.Init(how, 0);

	// send logout message, disable async, close socket - don't care about errors
	sendbuf.ReadMessage(level_logout_msg);
	send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

		//_tprintf("logged out at time %d\n",LyraTime());

	logged_into_level = false;
	num_packets_expected = num_packets_received = 0;

	player->SetItemNeedFlagsOrSortingUpdate(false);

	this->CheckInvariants(__LINE__);
	return;
}

void cGameServer::Logout(int how, bool final_logout)
{
	cItem *item;
	GMsg_Logout logout_msg;
#ifndef AGENT
	this->UpdateServer();
#endif

	// important: do NOT send level logout message when leaving the game!!!
	if (logged_into_game)
	{	
#ifndef AGENT
		if (player->Avatar().Hidden())  
		{
			LmAvatar temp = player->Avatar();
			temp.SetHidden(0);
			player->SetAvatar(temp, true);
		}
#endif
		// now normalize the item sort indexes
		cItem* (inventory[Lyra::INVENTORY_MAX]);
#ifndef AGENT
		int num_owned_items = SortInventory((cItem**)&inventory);
#else
		int num_owned_items = 0;
#endif
			
		logout_msg.Init(how, num_owned_items);

		for (int i=0; i<num_owned_items; i++)
			logout_msg.SetItem(i, inventory[i]->Lmitem().Header().Serial(), i+1, (unsigned char)(inventory[i]->InventoryFlags()));


		sendbuf.ReadMessage(logout_msg);
		send (sd_game, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
	}

	if (party)
	{
		if (party->RequestOutstanding() != Lyra::ID_UNKNOWN)
			party->RejectRequest();
		party->DissolveParty(false);
		delete party; party = NULL;
	}


	if (sd_game)
	{

//#if (defined(UL_DEBUG) || defined(ALT))
//				LoadString (hInstance, IDS_SOCKETCLOSING1, message, sizeof(message));
//				display->DisplayMessage(message);
//#endif
		// disable async for close to avoid unnecessary close message
		if (cDD && cDD->Hwnd_Main())
			WSAAsyncSelect( sd_game, cDD->Hwnd_Main(), WM_GAME_SERVER_DATA, 0);

		int iRc;
		if (!final_logout)
		{

			// reset to nonblocking mode before calling closesocket
			unsigned long blocking = 0;
			iRc = ioctlsocket(sd_game, FIONBIO, &blocking);
			if (iRc == SOCKET_ERROR)
			{
				SOCKETS_ERROR(iRc);
				return;
			}
			

			linger my_linger;
			my_linger.l_linger = 1; // 1 second timeout
			my_linger.l_onoff = 1;
			iRc = setsockopt(sd_game, SOL_SOCKET, SO_LINGER, 
				(const char*)&my_linger, sizeof(linger));
			if (iRc == SOCKET_ERROR)
			{
				SOCKETS_ERROR(iRc);
				return;
			}

#if 0
			char buffer[1024];
			while (1) 
			{	// be sure there is no more data to read
				iRc = recv(sd_game, (char*)buffer, 1024, MSG_PEEK);
				if (iRc == 0)
					break; // finished
				iRc = recv(sd_game, (char*)buffer, 1024, 0);
				if (iRc == SOCKET_ERROR)
				{
					if (logged_into_game) {
						SOCKETS_ERROR(iRc);
					}
					break;
				}
			}

#endif
			
//#if (defined(UL_DEBUG) || defined(ALT))
//			LoadString (hInstance, IDS_SOCKETCLOSING2, message, sizeof(message));
//			display->DisplayMessage(message);
//#endif
		}

		iRc = closesocket(sd_game);
		if (iRc == SOCKET_ERROR)
		{
			SOCKETS_ERROR(iRc);
			return;
		}

//#if (defined(UL_DEBUG) || defined(ALT))
//				LoadString (hInstance, IDS_SOCKETCLOSING3, message, sizeof(message));
//				display->DisplayMessage(message);
//#endif

		sd_game = NULL;
	}

	// delete all items
	if(actors)
	{
		for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
			item->SetTerminate();
		actors->IterateItems(DONE);

#ifdef _DEBUG
		if (actors)
		{
			_tprintf(_T("WARNING: Logout: After iterating to delete all actors, actors remain (non-NULL)"));
// 			delete actors; // mket
		}
#endif

	}

	logged_into_game = false;
	connected_to_gs = false;

	return;
}

// takes in a pointer to an array of item pointers; fills it with pointers to the
// items in the player's inventory, and then sorts them based on their position
// in the player's pack
int cGameServer::SortInventory(cItem** inventory)
{
#ifdef AGENT
	return 0; // agents don't worry about inventory
#endif

	int num_owned_items = 0;
	cItem* item;

	for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
		if (item->Status() == ITEM_OWNED)
		{
			inventory[num_owned_items] = item;
			num_owned_items++;
		} 
	actors->IterateItems(DONE);
	qsort(inventory, num_owned_items, sizeof(cItem*), CompareItemSortIndex);

	return num_owned_items;


}

// problem with login
void cGameServer::ServerError(TCHAR *error_message)
{
	// we need to set it here, otherwise the message pump for the dialog box
	// may cause problems by trying to draw things, etc. when we're exiting
	exiting = true;
#ifdef AGENT
	output->Write(error_message);
	GAME_ERROR(error_message);
#else
	_tcscpy(message, error_message);
	LyraDialogBox(hInstance, IDD_FATAL_ERROR, NULL, (DLGPROC)FatalErrorDlgProc);
	Exit();
#endif
	return;
}


void cGameServer::SetAvatarDescrip(TCHAR* description)
{
//_tcscpy(avatar_descrip, description);
  // Jared 2-17-00
  _tcsnccpy(avatar_descrip, description, sizeof(avatar_descrip));
  TRUNC(avatar_descrip, sizeof(avatar_descrip));
}

bool cGameServer::MPGPLogTime(int sleep_interval)
{
#ifndef AGENT

	long sessionid = this->MPSessionID();
	HINTERNET hInternet = this->HInternet();
	TCHAR mp_URL[256];
	const int buffer_length = 128;
	char timestamp_text[buffer_length];
	unsigned long length;
	long result;
	static int num_errors = 0;
	bool done = false;
	int tries = 10;

	LoadString (hInstance, IDS_MULTIPLAYER_URL_CHECKIN, disp_message, sizeof(disp_message));
	_stprintf(mp_URL, disp_message, sessionid);
	HINTERNET hMPGamePassResponse;

	// try to log time
	int i = 0;
	for (i=0; i< tries; i++)
	{
		hMPGamePassResponse = InternetOpenUrl(hInternet, mp_URL, NULL, NULL, INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE, 1);
		if (!hMPGamePassResponse)
		{
			Sleep(sleep_interval);
			num_errors++;
		}
		else
			break;
	}

	if (50 == num_errors) // too many failures; end session
	{
		LoadString (hInstance, IDS_MP_LOG_TIME_ERROR, disp_message, sizeof(message));
		this->ServerError(disp_message);
		return false;
	}

	if (i == tries)
		return false;

	for (i=0; i<tries; i++)		
	{	//URL has been opened, initiate file read
		if (!InternetReadFile(hMPGamePassResponse, (LPVOID *)timestamp_text, buffer_length, &length))
		{
				Sleep(sleep_interval);
				num_errors++;
		}
		else
			{
				result = atoi(timestamp_text);
				if (result != 1)
				{
					Sleep(sleep_interval);
					num_errors++;
				}
				else
					break;
			}
		}
	InternetCloseHandle(hMPGamePassResponse);

	if (50 == num_errors) // too many failures; end session
	{
		LoadString (hInstance, IDS_MP_LOG_TIME_ERROR, disp_message, sizeof(message));
		this->ServerError(disp_message);
		return false;
	}

#endif // AGENT
	return true;
}


// Destructor
cGameServer::~cGameServer()
{
#ifdef AGENT
	TlsSetValue(tlsGS, NULL);
#endif AGENT

	if (sd_udp != NULL)
		closesocket(sd_udp);

	this->Logout(GMsg_Logout::LOGOUT_NORMAL, true);

#ifndef AGENT
	KillTimer(cDD->Hwnd_Main(), TIMER_PLAYER_STATS_UPDATE );
#endif
}


#ifdef CHECK_INVARIANTS
void cGameServer::CheckInvariants(int line)
{

}
#endif
