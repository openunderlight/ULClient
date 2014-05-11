// GMsg_LocateMaresAck.h  -*- C++ -*-
// $Id: GMsg_LocateMaresAck.h,v 1.3 1998-02-26 14:35:44-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// locate player

#ifndef INCLUDED_GMsg_LocateMaresAck
#define INCLUDED_GMsg_LocateMaresAck

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_LocateMaresAck : public LmMesg {

public:

  enum {
    // constants
    MAX_PLAYERS = 10
  };

public:

  GMsg_LocateMaresAck();
  ~GMsg_LocateMaresAck();

  void Init(int num_players);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int NumPlayers() const;
  const TCHAR* PlayerName(int player_num) const;
  short LevelID(int player_num) const;
  short RoomID(int player_num) const;

  // mutators
  void SetNumPlayers(int num_players);
  void SetPlayerName(int player_num, const TCHAR* playername);
  void SetLocation(int player_num, short levelid, short roomid);

public: // logically private

  // player info structure
  struct pinfo_t {
    TCHAR playername[Lyra::PLAYERNAME_MAX];
    short levelid;
    short roomid;
  };

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
    pinfo_t players[MAX_PLAYERS];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_LocateMaresAck.i"
#endif

#endif /* INCLUDED_GMsg_LocateMaresAck */
