// RMsg_EnterRoom.h  -*- C++ -*-
// $Id: RMsg_EnterRoom.h,v 1.15 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// enterroom message

#ifndef INCLUDED_RMsg_EnterRoom
#define INCLUDED_RMsg_EnterRoom

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"
#include "RmRemotePlayer.h"

// forward references

// message class

class RMsg_EnterRoom : public LmMesg {

public:

  enum {
    // constants
    MAX_PLAYERS = Lyra::ROOMPEOPLE_MAX,

    // defaults
    DEFAULT_NUMPLAYERS = 0
  };

public:

  RMsg_EnterRoom();
  ~RMsg_EnterRoom();

  void Init(int num_players);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int NumPlayers() const;
  const RmRemotePlayer& Player(int num) const;

  // mutators
  void SetNumPlayers(int np);
  void SetPlayer(int num, const RmRemotePlayer& p);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  void calc_size();
  void calc_players();

  int num_players_;  // number of players in data_.players

  // message data structure
  struct data_t {
    RmRemotePlayer players[MAX_PLAYERS];
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_EnterRoom.i"
#endif

#endif /* INCLUDED_RMsg_EnterRoom */
