// GMsg_GetPlayerName.cpp  -*- C++ -*-
// $Id: GMsg_GetPlayerName.cpp,v 1.1 1998-04-24 16:18:04-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_GetPlayerName.h"
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

#include "GMsg_GetPlayerName.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_GetPlayerName.i"
#endif

////
// constructor
////

GMsg_GetPlayerName::GMsg_GetPlayerName()
  : LmMesg(GMsg::GETPLAYERNAME, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0, 0);
}

////
// destructor
////

GMsg_GetPlayerName::~GMsg_GetPlayerName()
{
  // empty
}

////
// Init
////

void GMsg_GetPlayerName::Init(lyra_id_t playerid, int requestid)
{
  SetPlayerID(playerid);
  SetRequestID(requestid);
}

////
// hton
////

void GMsg_GetPlayerName::hton()
{
  HTONL(data_.playerid);
  HTONL(data_.requestid);
}

////
// ntoh
////

void GMsg_GetPlayerName::ntoh()
{
  NTOHL(data_.playerid);
  NTOHL(data_.requestid);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_GetPlayerName::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_GetPlayerName[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("data>\n"));
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
