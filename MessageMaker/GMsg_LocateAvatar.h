// GMsg_LocateAvatar.h  -*- C++ -*-
// $Id: GMsg_LocateAvatar.h,v 1.4 1997-12-05 14:44:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// locate player request

#ifndef INCLUDED_GMsg_LocateAvatar
#define INCLUDED_GMsg_LocateAvatar

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_LocateAvatar : public LmMesg {

public:

  enum {
    MAX_PLAYERS = 12  // max players that can be requested
  };

public:

  GMsg_LocateAvatar();
  ~GMsg_LocateAvatar();

  void Init(int num_players);
  void Init(const TCHAR* playername); // compatibility: for single player

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int NumPlayers() const;
  const TCHAR* PlayerName(int player_num) const;

  // mutators
  void SetNumPlayers(int num_players);
  void SetPlayerName(int player_num, const TCHAR* playername);

public: // logically private

  // player name type
  typedef TCHAR pname_t[Lyra::PLAYERNAME_MAX];

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
    pname_t players[MAX_PLAYERS];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_LocateAvatar.i"
#endif

#endif /* INCLUDED_GMsg_LocateAvatar */
