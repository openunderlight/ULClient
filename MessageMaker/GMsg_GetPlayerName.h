// GMsg_GetPlayerName.h  -*- C++ -*-
// $Id: GMsg_GetPlayerName.h,v 1.1 1998-04-24 16:18:04-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// request player name from server

#ifndef INCLUDED_GMsg_GetPlayerName
#define INCLUDED_GMsg_GetPlayerName

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// message class

class GMsg_GetPlayerName : public LmMesg {

public:

  GMsg_GetPlayerName();
  ~GMsg_GetPlayerName();

  void Init(lyra_id_t playerid, int requestid);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  int RequestID() const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetRequestID(int requestid);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
    int requestid;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_GetPlayerName.i"
#endif

#endif /* INCLUDED_GMsg_GetPlayerName */
