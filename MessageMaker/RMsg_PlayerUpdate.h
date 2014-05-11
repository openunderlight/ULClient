// RMsg_PlayerUpdate.h  -*- C++ -*-
// $Id: RMsg_PlayerUpdate.h,v 1.9 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// server->client peer update message

#ifndef INCLUDED_RMsg_PlayerUpdate
#define INCLUDED_RMsg_PlayerUpdate

#ifdef __GNUC__
#pragma interface
#endif

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"
#include "LmPeerUpdate.h"

// forward references

// message class

class RMsg_PlayerUpdate : public LmMesg {

public:

  enum {
    // constants
    MAX_PLAYERS = Lyra::ROOMPEOPLE_MAX,

    // default values
    DEFAULT_NUMPLAYERS = 0
  };

public:

  RMsg_PlayerUpdate();
  ~RMsg_PlayerUpdate();

  void Init(lyra_id_t playerid, int num_players);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  int NumPlayers() const;
  const LmPeerUpdate& PeerUpdate(int num) const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetNumPlayers(int num_players);
  void SetPeerUpdate(int num, const LmPeerUpdate& update);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  void calc_size();
  void calc_players();

  int num_players_;

  // message data structure
  struct data_t {
    lyra_id_t playerid;
    LmPeerUpdate players[MAX_PLAYERS];
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_PlayerUpdate.i"
#endif

#endif /* INCLUDED_RMsg_PlayerUpdate */
