// RMsg_GetRoomDescription.h  -*- C++ -*-
// $Id: RMsg_GetRoomDescription.h,v 1.1 1998-04-17 16:27:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// request avatar description

#ifndef INCLUDED_RMsg_GetRoomDescription
#define INCLUDED_RMsg_GetRoomDescription

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"

// message class

class RMsg_GetRoomDescription : public LmMesg {

public:

  RMsg_GetRoomDescription();
  ~RMsg_GetRoomDescription();

  void Init(short levelid, short roomid);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  short LevelID() const;
  short RoomID() const;

  // mutators
  void SetLevelID(short level);
  void SetRoomID(short room);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    short levelid;
	short roomid;
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_GetRoomDescription.i"
#endif

#endif /* INCLUDED_RMsg_GetRoomDescription */
