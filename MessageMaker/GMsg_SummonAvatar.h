// GMsg_SummonAvatar.h  -*- C++ -*-
// $Id: GMsg_SummonAvatar.h,v 1.4 1997-12-05 14:44:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// Summon player request

#ifndef INCLUDED_GMsg_SummonAvatar
#define INCLUDED_GMsg_SummonAvatar

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_SummonAvatar : public LmMesg {

public:

  enum {
    MAX_PLAYERS = 24  // max players that can be requested
  };

public:

  GMsg_SummonAvatar();
  ~GMsg_SummonAvatar();

  void Init(int num_players);
  void Init(const char* playername); // compatibility: for single player

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int NumPlayers() const;
  const char* PlayerName(int player_num) const;

  // mutators
  void SetNumPlayers(int num_players);
  void SetPlayerName(int player_num, const char* playername);

public: // logically private

   typedef char pname_t[Lyra::PLAYERNAME_MAX];
	struct pinfo_t
	{
		pname_t playername;
		int status;
      short x;
      short y;
      short height;
      unsigned short angle;
		lyra_id_t levelid;
		lyra_id_t roomid;
	};

  // player name type

private:

  // standard non-public methods
  void hton();
  void ntoh();

  void calc_size();
  void calc_players();

  // non-message data
  int num_players_;

  // message data structure
  struct data_t {
	 pinfo_t castor_info;
    pname_t players[MAX_PLAYERS];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_SummonAvatar.i"
#endif

#endif /* INCLUDED_GMsg_SummonAvatar */
