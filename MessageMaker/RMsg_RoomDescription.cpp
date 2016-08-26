// RMsg_RoomDescription.cpp  -*- C++ -*-
// $Id: RMsg_RoomDescription.cpp,v 1.1 1998-04-17 16:27:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_RoomDescription.h"
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

#include "RMsg_RoomDescription.h"
#include "LyraDefs.h"
#include "RMsg.h"

#ifndef USE_INLINE
#include "RMsg_RoomDescription.i"
#endif

////
// constructor
////

RMsg_RoomDescription::RMsg_RoomDescription()
  : LmMesg(RMsg::ROOMDESCRIPTION, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0, 0, _T("none"));
}

////
// destructor
////

RMsg_RoomDescription::~RMsg_RoomDescription()
{
  // empty
}

////
// Init
////

void RMsg_RoomDescription::Init(short levelid, short roomid, const TCHAR* description)
{
  SetLevelID(levelid);
  SetRoomID(roomid);
  SetDescription(description);
}

////
// hton
////

void RMsg_RoomDescription::hton()
{
  HTONS(data_.levelid);
  HTONS(data_.roomid);
  // not converted: Description
}

////
// ntoh
////

void RMsg_RoomDescription::ntoh()
{
  NTOHS(data_.levelid);
  NTOHS(data_.roomid);
  // not converted: Description
  calc_size();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_RoomDescription::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_RoomDescription[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
	  _ftprintf(f, _T("level;room = %d;%d desc='%s'>\n"), LevelID(), RoomID(), Description());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetDescription
////

void RMsg_RoomDescription::SetDescription(const TCHAR* description)
{
 _tcsnccpy(data_.description, description, sizeof(data_.description));
  TRUNC(data_.description, sizeof(data_.description));
  calc_size();
}

////
// calc_size
////

void RMsg_RoomDescription::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.description);
  // add string length, plus 1 for null
  size += (_tcslen(data_.description) + 1);
  SetMessageSize(size);
}
