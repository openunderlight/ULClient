#ifndef GAMESERVER_H
#define GAMESERVER_H

// Copyright Lyra LLC, 1996. All rights reserved. 

#include "Central.h"
#include <winsock2.h>
#include "LmMessage.h"
#include "cActor.h"
#include "GMsg_All.h"
#include "RMsg_All.h"
#include <wininet.h>

class cItem;
class cNeighbor;
class cParty;

//////////////////////////////////////////////////////////////////
// Constants

const unsigned short	DEFAULT_UDP_PORT=9599;
//const int GAME_CURRENT_VERSION = 13;
const unsigned int	SHOT_INTERVAL=1200; // milliseconds between shots
const unsigned int	HIT_INTERVAL=1000; // milliseconds between hits
const unsigned int  ITEM_USE_INTERVAL=500; // milliseconds between item use

#define VERSION_KEY "SOFTWARE\\Lyra\\Underlight"

enum // types of triggers for peer update
{
    TRIGGER_TIMER, TRIGGER_ATTACK, TRIGGER_MOVE, TRIGGER_DEATH
};

enum // types of logins
{
    LOGIN_NORMAL, LOGIN_AGENT
};


struct last_attack_t
{
	int		time;
	int		height_delta;
	int		bitmap;
	int		damage;
	int		effect;
	int		velocity;
};

struct alert_t {
	TCHAR playerName[Lyra::PLAYERNAME_MAX];
	DWORD alertTime;
};

// make manipulation of goal detail message easier
typedef TCHAR pname_t[Lyra::PLAYERNAME_MAX];

///////////////////////////////////////////////////////////////////
// Additional Windows Messages

#define WM_GAME_SERVER_DATA			WM_USER + GS_MAGIC + 1
#define WM_POSITION_UPDATE				WM_USER + GS_MAGIC + 2
#define TIMER_POSITION_UPDATE			WM_USER + GS_MAGIC + 3
#define TIMER_PLAYER_STATS_UPDATE	WM_USER + GS_MAGIC + 4
#define WM_GM_QUERY						WM_USER + GS_MAGIC + 6


///////////////////////////////////////////////////////////////////
// Structures


//////////////////////////////////////////////////////////////////
// Function Prototypes

int __cdecl CompareItemSortIndex(const void *elem1, const void *elem2);

//////////////////////////////////////////////////////////////////
// Class Definition


class cGameServer
   {
   public:

   private:
	   SOCKET		sd_game, sd_udp; 
	   unsigned	short udp_port; // local port # for UDP updates
	   unsigned short agent_gs_port; // game server port for agents
//	   in_addr		localIP; // local IP address
	   SOCKADDR_IN  game_server_addr; // IP address/port of game server
	   SOCKADDR_IN  level_server_addr; // IP address/port of level server
	   bool			logged_into_game, logged_into_level; // true after login_ack
	   bool		    has_logged_in; // used to track pmare billing
	   bool			connected_to_gs; // true after gs connection made
	   int			curr_level_id;
	   int			portNumber; // port # for position updates
	   cParty		*party; 
	   int			login_type; // agent or normal
	   int			version; // version # of server software
	   int			header_bytes_read, body_bytes_read;
	   bool			reading_header;
	   LmMesgBuf	msgbuf; // message buffer used to read in network messages
	   LmMesgBuf	sendbuf; // message buffer used to send messages
	   LmMesgHdr	msgheader; // message header
		// real-time update member variables 
	   bool			preupdate; // have we pre-sent local group update info?
	   bool			jumped; // have we jumped since the last update?
	   cItem*		creating_item; // are we in the process of creating an item?
	   int          attack_bits : 5; // attack bits
	   int			hit_bits : 5; // hit bits
	   int			strafing; // moving directly left or right
	   int			last_peer_update; // time last peer updates were sent
	   last_attack_t last_attack;
	   DWORD	    last_item_use_time; // time of last talisman access
	   bool			displayed_item_use_message;
	   int			last_sound; 
	   int			num_packets_expected;
	   int			num_packets_received;
	   int num_packets_in, num_packets_out; // for profiling network usage
	   DWORD		begin_time, ping_time; 
	   LmPeerUpdate last_update;
	   bool			auto_level_login; // auto login into level after gs login
	   bool			logged_into_room; // have received 1st item drop message for room
	   bool			logging_in; // in the process of login; used for error handling
	   bool			got_peer_updates; // after entering a new room, can't use items/arts until we get a UDP update
	   char			rw_host[16]; // roger wilco address
	   int			login_attempts; // to track # of game full messages
	   TCHAR			avatar_descrip[Lyra::MAX_AVATARDESC];
	   unsigned int build; // build number, modified by PMARE/GAMEMASTER
	   HINTERNET hInternet, hVersionFile, hMPGamePassResponse; 
	   bool			game_full;
	   bool			loading_inventory;
	   bool			game_server_full[64]; //ROUND_ROBIN 
	   DWORD		room_change_time;
	   long			mp_sessionid;
	   int			last_room_target;
	   int			last_level_target;
	   int			alert_count;

   public:
      cGameServer(unsigned short udp_port_num = DEFAULT_UDP_PORT, unsigned short gs_port_num = 0);
	  ~cGameServer(void);

	  inline void SetServerAddress(SOCKADDR_IN value) { game_server_addr = value; }
	  inline SOCKADDR_IN ServerAddress(void) { return game_server_addr; }
	  inline void SetAutoLevelLogin(bool value) { auto_level_login = value; }
	  inline void SetLoggedIntoRoom(bool flag = true) { logged_into_room = flag; }
	  inline bool LoggedIntoRoom(void) { return logged_into_room; }
	  inline TCHAR* AvatarDescrip(void) { return avatar_descrip; }
	  void SetAvatarDescrip(TCHAR* description); 
	
	  // real-time update methods
	  inline void SetJump (void) { jumped = TRUE; };
	  bool PlayerAttack(int bitmap_id, int velocity, int effect, int damage_type, cItem *item = NO_ACTOR,
		                int art_id = Arts::NONE); 
	  inline int NumPacketsOut(void) { return num_packets_out; };
	  inline int NumPacketsIn(void) { return num_packets_in; };
	  bool GotPeerUpdates(void) { return got_peer_updates; };
	  inline void SetLastSound(int sound) { last_sound = sound; };

	  // received input from server - tcp and udp
	  void OnServerUpdate(HWND hWindow, WPARAM wParam, LPARAM lParam);
	  void HandleMessage(void);
	  void OnPositionUpdate(HWND hWindow, WPARAM wParam, LPARAM lParam);
	  void HandlePositionUpdate(RMsg_PlayerUpdate& position_msg);

	  // login/logout methods
	  void Login(int type = LOGIN_NORMAL); // game login
	  void Logout(int how, bool final_logout);
	  void LevelLogin(void);
	  void LevelLogout(int how);
	  int SortInventory(cItem** inventory);

	  // player creation related methods
	  void WelcomeAIComplete(void);

	  // item methods
	  bool GetItem(cItem *item);
	  bool CreateItem(cItem *item, int ttl = GMsg_PutItem::DEFAULT_TTL, TCHAR *description = NULL);
	  bool DropItem(cItem *item);
	  bool DestroyItem(cItem *item);
	  bool GiveItem(cItem *item, cNeighbor *n);
	  bool ShowItem(cItem *item, cNeighbor *n);
	  bool TakeItemAck(int status, cItem *item);
	  bool CanUseItem(void);

	  // goal methods
	  void PostGoal(realmid_t goalid, int rank, int guild, int maxaccepted, int expirationdays, 
		  int sugsphere, int sugstat, int guardian, TCHAR* summary, TCHAR* goaltext, TCHAR* keywords,
		  unsigned int graphic, unsigned char num_items, unsigned char color1, 
		  unsigned char color2,  unsigned char item_type, unsigned int field1, 
		  unsigned int field2, unsigned int field3,  
		  unsigned int quest_xp);
	  void PostReport(realmid_t goal_id, int awardxp, TCHAR* recipient, 
		  TCHAR* summary, TCHAR *report);
	  void RequestGoalbook(void);
	  void RequestGoalHeaders(int sessionid, int guild, int rank, realmid_t lastseen);
	  void RequestGoalText(realmid_t goal);
	  void RequestGoalDetails(realmid_t goal);
	  void RequestGoalGuardianFlag(realmid_t goal);
	  void RequestReportHeaders(realmid_t goal_id, int guild, int rank, int sessionid, realmid_t lastseen);
	  void RequestReportText(realmid_t report);
	  void AcceptGoal(realmid_t goal);
	  void DeleteGoal(realmid_t goal);
	  void DeleteReport(realmid_t report);
	  void RemoveFromGoalbook(realmid_t goal);
	  void CompleteGoal(realmid_t goal);
	  void IsCodexQuestCompleted(lyra_id_t quest);
	  void CompleteQuest(realmid_t quest);
	  void VoteGoal(realmid_t goal, int vote);

	  // game server stat update
	  void UpdateServer(void);

	  // room server updates
	  void OnRoomChange(short last_x, short last_y);
	  void Talk(TCHAR *talk, int speechType, realmid_t target, bool echo = false, bool allow_babble = true); // say something
	  void JoinParty(realmid_t playerID, bool auto_rejoin = false); // join another player's party
	  void RejectPartyQuery(int reason, realmid_t playerID); // reject request
	  void AcceptPartyQuery(realmid_t playerID); // accept request
	  void SendPlayerMessage(lyra_id_t destination_id, short msg_type, short param1, short param2);
	  void LeaveParty(realmid_t playerID); // leave the current party
	  void AvatarChange(LmAvatar new_avatar, bool permanent);
	  void LocateAvatar(GMsg_LocateAvatar& locate_msg);

	  // timer-driven position update
	  void SendPositionUpdate (int trigger);

	  // information gathering methods
	  void FindRoomPopulations(lyra_id_t level_id);
	  void PingServer(void);
	  void SendItemDescripRequest(LmItemHdr& itemheader);
	  void SendAvatarDescrip(void);
	  void GetAvatarDescrip(lyra_id_t player_id);
	  void GetRoomDescrip(int levelid, int roomid);

	  void UsePPoints(short how, short var1, short var2, short var3);
	  void GrantPPoint(lyra_id_t target_id, TCHAR* why);

	  // security measures
	  void UpdateExpectedPackets(DWORD interval);
	  void CheckUDPConnectivity(void);


	  // Selectors
	  inline bool LoggedIntoGame(void) { return logged_into_game; };
	  inline bool LoggedIntoLevel(void) { return logged_into_level; };
	  inline bool HasLoggedIn(void) { return has_logged_in; };
	  inline int Version(void) { return version;};
	  inline cParty* Party(void) { return party; };
	//  inline in_addr LocalIP()   { return localIP;   };
	  inline char* RWHost() { return rw_host; };
	  inline DWORD LastAttackTime() { return last_attack.time; };
	  inline HINTERNET HInternet() { return hInternet; };
	  inline long MPSessionID() { return mp_sessionid; };

	  // Other Stuff
	  void ServerError(TCHAR *error_message);
	  bool MPGPLogTime(int sleep_interval);

   private:
	  void InitUDPSocket(void);
	  void FillInPlayerPosition(LmPeerUpdate *update, int trigger = TRIGGER_TIMER);

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cGameServer(const cGameServer& x);
	cGameServer& operator=(const cGameServer& x);

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif


   };
#endif
