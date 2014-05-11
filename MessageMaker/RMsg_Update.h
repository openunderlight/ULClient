// RMsg_Update.h  -*- C++ -*-
// $Id: RMsg_Update.h,v 1.5 1997-09-16 03:02:25-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// CS player update message

#ifndef INCLUDED_RMsg_Update
#define INCLUDED_RMsg_Update

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"
#include "LmPeerUpdate.h"

// forward references

// message class

class RMsg_Update : public LmMesg {

public:

  RMsg_Update();
  ~RMsg_Update();

  void Init(lyra_id_t playerid, const LmPeerUpdate& update);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  const LmPeerUpdate& PeerUpdate() const;

  // selectors returning mutable data
  LmPeerUpdate& PeerUpdate();

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetPeerUpdate(const LmPeerUpdate& update);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    LmPeerUpdate update;
	lyra_id_t playerid;
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_Update.i"
#endif

#endif /* INCLUDED_RMsg_Update */
