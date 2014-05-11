// GMsg_RcvPlayerName.h  -*- C++ -*-
// $Id: GMsg_RcvPlayerName.h,v 1.1 1998-04-24 16:18:04-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// return player name to client

#ifndef INCLUDED_GMsg_RcvPlayerName
#define INCLUDED_GMsg_RcvPlayerName

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// message class

class GMsg_RcvPlayerName : public LmMesg {

public:

  GMsg_RcvPlayerName();
  ~GMsg_RcvPlayerName();

  void Init(lyra_id_t playerid, int requestid, const TCHAR* playername);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  int RequestID() const;
  const TCHAR* PlayerName() const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetRequestID(int requestid);
  void SetPlayerName(const TCHAR* playername);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
    int requestid;
    TCHAR playername[Lyra::PLAYERNAME_MAX];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_RcvPlayerName.i"
#endif

#endif /* INCLUDED_GMsg_RcvPlayerName */
