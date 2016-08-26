// RMsg_RoomDescription.h  -*- C++ -*-
// $Id: RMsg_RoomDescription.h,v 1.1 1998-04-17 16:27:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// receive avatar description from server

#ifndef INCLUDED_RMsg_RoomDescription
#define INCLUDED_RMsg_RoomDescription

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"

// message class

class RMsg_RoomDescription : public LmMesg {

public:

  RMsg_RoomDescription();
  ~RMsg_RoomDescription();

  void Init(short levelid, short roomid, const TCHAR* description);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  short LevelID() const;
  short RoomID() const;
  const TCHAR* Description() const;

  // mutators
  void SetLevelID(short levelid);
  void SetRoomID(short roomid);
  void SetDescription(const TCHAR* description);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual message size
  void calc_size();

  // message data structure
  struct data_t {
    short levelid;
	short roomid;
    TCHAR description[Lyra::ROOMDESC_MAX]; 
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_RoomDescription.i"
#endif

#endif /* INCLUDED_RMsg_RoomDescription */
