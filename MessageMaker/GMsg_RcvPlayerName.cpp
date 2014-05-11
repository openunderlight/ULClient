// GMsg_RcvPlayerName.cpp  -*- C++ -*-
// $Id: GMsg_RcvPlayerName.cpp,v 1.1 1998-04-24 16:18:04-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_RcvPlayerName.h"
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

#include "GMsg_RcvPlayerName.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_RcvPlayerName.i"
#endif

////
// constructor
////

GMsg_RcvPlayerName::GMsg_RcvPlayerName()
  : LmMesg(GMsg::RCVPLAYERNAME, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0, 0, _T("nobody"));
}

////
// destructor
////

GMsg_RcvPlayerName::~GMsg_RcvPlayerName()
{
  // empty
}

////
// Init
////

void GMsg_RcvPlayerName::Init(lyra_id_t playerid, int requestid, const TCHAR* playername)
{
  SetPlayerID(playerid);
  SetRequestID(requestid);
  SetPlayerName(playername);
}

////
// hton
////

void GMsg_RcvPlayerName::hton()
{
  HTONL(data_.playerid);
  HTONL(data_.requestid);
  // not converted: PlayerName
}

////
// ntoh
////

void GMsg_RcvPlayerName::ntoh()
{
  NTOHL(data_.playerid);
  NTOHL(data_.requestid);
  // not converted: PlayerName
}

////
// SetPlayerName
////

void GMsg_RcvPlayerName::SetPlayerName(const TCHAR* playername)
{
 _tcsnccpy(data_.playername, playername, sizeof(data_.playername));
  TRUNC(data_.playername, sizeof(data_.playername));
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_RcvPlayerName::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_RcvPlayerName[%p]: "), this);
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
