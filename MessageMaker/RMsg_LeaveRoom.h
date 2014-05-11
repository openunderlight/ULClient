// RMsg_LeaveRoom.h  -*- C++ -*-
// $Id: RMsg_LeaveRoom.h,v 1.12 1997-07-18 17:26:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// message class template

#ifndef INCLUDED_RMsg_LeaveRoom
#define INCLUDED_RMsg_LeaveRoom

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"

// forward references

// message class

class RMsg_LeaveRoom : public LmMesg {

public:

  enum {
    // how they left the room
    UNKNOWN = 'U',  // invalid

    DEATH   = 'D',  // was killed
    LOGOUT  = 'L',  // logged out
    NORMAL  = 'N'   // left by going to another room
  };

public:

  RMsg_LeaveRoom();
  ~RMsg_LeaveRoom();

  void Init(lyra_id_t playerid, int status, int lastx, int lasty);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  int LastX() const;
  int LastY() const;
  int Status() const;

  // mutators
  void SetStatus(int status);
  void SetPlayerID(lyra_id_t playerid);
  void SetLastPosition(int lastx, int lasty);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
    int status;
    short lastx;
    short lasty;
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_LeaveRoom.i"
#endif

#endif /* INCLUDED_RMsg_LeaveRoom */
