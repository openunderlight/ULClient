// RMsg_GetRoomDescription.cpp  -*- C++ -*-
// $Id: RMsg_GetRoomDescription.cpp,v 1.1 1998-04-17 16:27:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_GetRoomDescription.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock2.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */
#include <stdio.h>
#include <string.h>

#include "RMsg_GetRoomDescription.h"
#include "LyraDefs.h"
#include "RMsg.h"

#ifndef USE_INLINE
#include "RMsg_GetRoomDescription.i"
#endif

////
// constructor
////

RMsg_GetRoomDescription::RMsg_GetRoomDescription()
  : LmMesg(RMsg::GETROOMDESCRIPTION, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0,0);
}

////
// destructor
////

RMsg_GetRoomDescription::~RMsg_GetRoomDescription()
{
  // empty
}

////
// Init
////

void RMsg_GetRoomDescription::Init(short levelid, short roomid)
{
  SetLevelID(levelid);
  SetRoomID(roomid);
}

////
// hton
////

void RMsg_GetRoomDescription::hton()
{
  HTONS(data_.levelid);
  HTONS(data_.roomid);
}

////
// ntoh
////

void RMsg_GetRoomDescription::ntoh()
{
  NTOHS(data_.levelid);
  NTOHS(data_.roomid);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_GetRoomDescription::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_GetRoomDescription[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
	  _ftprintf(f, _T("level; room = %d; %d>\n"), LevelID(), RoomID());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
