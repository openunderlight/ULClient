// GMsg_GotoLevel.h  -*- C++ -*-
// $Id: GMsg_GotoLevel.h,v 1.4 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// level login message

#ifndef INCLUDED_GMsg_GotoLevel
#define INCLUDED_GMsg_GotoLevel

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"
#include "LmPeerUpdate.h"

// forward references

// message class

class GMsg_GotoLevel : public LmMesg {

public:

  GMsg_GotoLevel();
  ~GMsg_GotoLevel();

  void Init(lyra_id_t levelid, lyra_id_t roomid, const LmPeerUpdate& update);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t LevelID() const;
  lyra_id_t RoomID() const;
  const LmPeerUpdate& PeerUpdate() const;

  //mutators
  void SetLevelID(lyra_id_t levelid);
  void SetRoomID(lyra_id_t roomid);
  void SetPeerUpdate(const LmPeerUpdate& update);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t levelid;
    lyra_id_t roomid;
    LmPeerUpdate update;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_GotoLevel.i"
#endif

#endif /* INCLUDED_GMsg_GotoLevel */
