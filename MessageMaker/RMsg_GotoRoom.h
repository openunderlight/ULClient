// RMsg_GotoRoom.h  -*- C++ -*-
// $Id: RMsg_GotoRoom.h,v 1.12 1997-07-18 17:26:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// goto another room

#ifndef INCLUDED_RMsg_GotoRoom
#define INCLUDED_RMsg_GotoRoom

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "LmPeerUpdate.h"

// forward references

// message class

class RMsg_GotoRoom : public LmMesg {

public:

  RMsg_GotoRoom();
  ~RMsg_GotoRoom();

  void Init(lyra_id_t roomid, const LmPeerUpdate& update, int lastx, int lasty);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t RoomID() const;
  int LastX() const;
  int LastY() const;
  const LmPeerUpdate& PeerUpdate() const;

  // mutators
  void SetRoomID(lyra_id_t roomid);
  void SetLastPosition(int lastx, int lasty);
  void SetPeerUpdate(const LmPeerUpdate& update);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t roomid;     // which room
    LmPeerUpdate update;  // position, other info
    short lastx;          // last (x,y) position in current room
    short lasty;
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_GotoRoom.i"
#endif

#endif /* INCLUDED_RMsg_GotoRoom */
