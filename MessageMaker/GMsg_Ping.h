// GMsg_Ping.h  -*- C++ -*-
// $Id: GMsg_Ping.h,v 1.3 1997-10-22 19:21:08-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// "ping" server message

#ifndef INCLUDED_GMsg_Ping
#define INCLUDED_GMsg_Ping

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_Ping : public LmMesg {

public:

  enum {
    PING_PLAYER_THREAD = 1,  // handled by player thread
    PING_GAME_THREAD,        // handled by game thread
    PING_PLAYER_DB,          // handled by player thread, performs player db transaction
    PING_ITEM_DB,            // handled by player thread, performs item db transaction
    PING_GUILD_DB            // handled by player thread, performs guild db transaction
  };

public:

  GMsg_Ping();
  ~GMsg_Ping();

  void Init(int nonce, int ping_type = PING_PLAYER_THREAD);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Nonce() const;
  int PingType() const;

  // mutators
  void SetNonce(int nonce);
  void SetPingType(int ping_type);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int nonce;
    int ping_type;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_Ping.i"
#endif

#endif /* INCLUDED_GMsg_Ping */
