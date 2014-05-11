// RMsg_NewlyAwakened.cpp  -*- C++ -*-
// $Id: RMsg_NewlyAwakened.cpp,v 1.1 1997-11-25 15:53:47-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_NewlyAwakened.h"
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

#include "RMsg_NewlyAwakened.h"
#include "LyraDefs.h"
#include "RMsg.h"

#ifndef USE_INLINE
#include "RMsg_NewlyAwakened.i"
#endif

////
// constructor
////

RMsg_NewlyAwakened::RMsg_NewlyAwakened()
  : LmMesg(RMsg::NEWLYAWAKENED, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init();
}

////
// destructor
////

RMsg_NewlyAwakened::~RMsg_NewlyAwakened()
{
  // empty
}

////
// Init
////

void RMsg_NewlyAwakened::Init(void)
{
	// empty
}

////
// hton
////

void RMsg_NewlyAwakened::hton()
{
    // not converted: PlayerName
    HTONL(data_.roomid);
}

////
// ntoh
////

void RMsg_NewlyAwakened::ntoh()
{
    // not converted: PlayerName
    NTOHL(data_.roomid);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_NewlyAwakened::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_NewlyAwakened[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
     _ftprintf(f, _T("player='%s' room=%u\n"), PlayerName(), RoomID());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetPlayerName
////

void RMsg_NewlyAwakened::SetPlayerName(const TCHAR* playername)
{
 _tcsnccpy(data_.playername, playername, sizeof(data_.playername));
  TRUNC(data_.playername, sizeof(data_.playername));
}
