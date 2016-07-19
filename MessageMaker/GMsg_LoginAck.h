// GMsg_LoginAck.h  -*- C++ -*-
// $Id: GMsg_LoginAck.h,v 1.21 1998-04-03 17:00:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// login acknowledge message

#ifndef INCLUDED_GMsg_LoginAck
#define INCLUDED_GMsg_LoginAck

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"
#include "LmItem.h"
#include "LmStats.h"
#include "LmAvatar.h"
#include "LmArts.h"

// forward references

// message class

class GMsg_LoginAck : public LmMesg {

public:

  enum {
    // login status values
    LOGIN_UNUSED       = 'U',  // invalid

    LOGIN_ALREADYIN    = 'A',  // user is already in game
    LOGIN_NO_BILLING   = 'B',  // free trial has expired
	LOGIN_NO_PMARE     = 'C',  // credit card account needed for pmare
    LOGIN_UNKNOWNERROR = 'E',  // unknown server error
    LOGIN_GAMEFULL     = 'F',  // game is full
    LOGIN_COOLOFF      = 'H',  // cooloff period between alts
	LOGIN_KILLED	   = 'K',  // player dead
	LOGIN_PMARE_LOCK   = 'L',  // PMare Global Lock in effect
	LOGIN_MISMATCH	   = 'M',  // account/build mismatch
    LOGIN_USERNOTFOUND = 'N',  // user not in database
    LOGIN_OK           = 'O',  // successful login
	LOGIN_PMARE_EXPIRED= 'Q',  // pmare session has expired
    LOGIN_BADPASSWORD  = 'P',  // password incorrect
	LOGIN_SUSPENDED    = 'S',  // player suspended
    LOGIN_TERMINATED   = 'T',  // player terminated
    LOGIN_WRONGVERSION = 'V',  // incorrect client version
    LOGIN_EXPIRED      = 'X',  // account has been expired
	LOGIN_MAX_PMARE	   = 'Z',  // pmare account maxed out for the day

	GAMESITE_LYRA	     = 'L',  // signs up directly through Lyra
	GAMESITE_MULTIPLAYER = 'M',  // via MultiPlayer.com
	GAMESITE_SKOTOS		 = 'S',  // via Skotos
	GAMESITE_NETEMP		 = 'N',  // via Netemp

    // default values
    DEFAULT_VERSION = 0,
    DEFAULT_NUMITEMS = 0
  };

public:

  GMsg_LoginAck();
  ~GMsg_LoginAck();

  void Init(int version, int status, lyra_id_t playerid, int server_port,
	    const LmStats& stats, const LmAvatar& avatar, const LmArts& arts,
	    int xp_gain, int xp_loss, int num_items, short max_minutes_online, 
		short session_minutes, unsigned short x, unsigned short y, 
		unsigned char level_id, unsigned char gamesite, lyra_id_t gamesite_id, 
		unsigned short ppoints, unsigned short pp_pool,
		const TCHAR* description);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Version() const;
  int Status() const;
  int Build() const;
  int SubBuild() const;

  lyra_id_t PlayerID() const;
  int ServerPort() const;
  const LmStats& PlayerStats() const;
  const LmAvatar& Avatar() const;
  const LmArts& Arts() const;
  int XPGained() const;
  int XPLost() const;
  int NumItems() const;
  const LmItem& Item(int itemnum) const;
  int ItemPosition(int itemnum) const;
  int ItemFlags(int itemnum) const;
  short MaxMinutesOnline() const;
  short SessionMinutes() const;
  short X() const;
  short Y() const;
  unsigned char LevelID() const;
  unsigned char Gamesite() const;
  short PPoints() const;
  short PPPool() const;
  lyra_id_t GamesiteID() const;
  //unsigned int LoginTime() const;
  const TCHAR* Description() const;


  // mutators
  void SetVersion(int version);
  void SetStatus(int status);
  void SetPlayerID(lyra_id_t playerid);
  void SetServerPort(int server_port);
  void SetPlayerStats(const LmStats& stats);
  void SetAvatar(const LmAvatar& avatar);
  void SetArts(const LmArts& arts);
  void SetXPGained(int xp_gain);
  void SetXPLost(int xp_loss);
  void SetNumItems(int num_items);
  void SetItem(int num, const LmItem& item, int x_value);
  void SetMaxMinutesOnline(short minutes);
  void SetSessionMinutes(short minutes);
  //void SetLoginTime(unsigned int login_time);
  void SetDescription(const TCHAR* description);
  void SetX(short x);
  void SetY(short y);
  void SetLevelID(unsigned char level_id);
  void SetGamesite(unsigned char gamesite);
  void SetPPoints(short value);
  void SetPPPool(short value);
  void SetGamesiteID(lyra_id_t gamesite_id);


public: // logically private

  struct item_state_t {
    LmItem item; // the item
    short pos;   // its relative position in inventory list (NOTE: only the low 8 bits are significant)
    short flags; // its flags (NOTE: only the low 8 bits are significant)
  };

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // valid index into inventory
  static bool item_num_ok(int item_num);

  // calculate actual size
  void calc_size();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
    short version;
    short request_status;
    LmStats stats;
    LmAvatar avatar;
    LmArts arts;
    int xp_gain;
    int xp_loss;
    short server_port;
    short num_items;
	short max_minutes_online;
	short session_minutes;
	short x;
	short y;
	unsigned char level_id; 
	unsigned char gamesite; 
	short ppoints;
	short pp_pool;
	unsigned short buffer2; // buffer to align on 4 byte boundary
	
    lyra_id_t gamesite_id;
	//unsigned int login_time; // UNIX login time, in secs since 1/1/1970
    TCHAR description[Lyra::MAX_AVATARDESC]; // player's description
    item_state_t items[Lyra::INVENTORY_MAX];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_LoginAck.i"
#endif

#endif /* INCLUDED_GMsg_LoginAck */
