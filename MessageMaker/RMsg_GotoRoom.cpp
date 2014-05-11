// RMsg_GotoRoom.cpp  -*- C++ -*-
// $Id: RMsg_GotoRoom.cpp,v 1.10 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_GotoRoom.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */
#include <stdio.h>
#include <string.h>

#include "RMsg_GotoRoom.h"
#include "RMsg.h"
#include "LyraDefs.h"

#ifndef USE_INLINE
#include "RMsg_GotoRoom.i"
#endif

////
// constructor
////

RMsg_GotoRoom::RMsg_GotoRoom()
  : LmMesg(RMsg::GOTOROOM, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, LmPeerUpdate::DEFAULT_INSTANCE, 0, 0);
}

////
// destructor
////

RMsg_GotoRoom::~RMsg_GotoRoom()
{
  // empty
}

////
// Init
////

void RMsg_GotoRoom::Init(lyra_id_t roomid, const LmPeerUpdate& update, int lastx, int lasty)
{
  SetRoomID(roomid);
  SetPeerUpdate(update);
  SetLastPosition(lastx, lasty);
}

////
// hton
////

void RMsg_GotoRoom::hton()
{
  HTONL(data_.roomid);
  HTONS(data_.lastx);
  HTONS(data_.lasty);
  data_.update.ConvertToNetwork();
}

////
// ntoh
////

void RMsg_GotoRoom::ntoh()
{
  NTOHL(data_.roomid);
  NTOHS(data_.lastx);
  NTOHS(data_.lasty);
  data_.update.ConvertToHost();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_GotoRoom::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_GotoRoom[%p,%d]: "), this, sizeof(RMsg_GotoRoom));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("roomid=%u lastpos=(%d,%d)>\n"), RoomID(), LastX(), LastY());
    PeerUpdate().Dump(f, indent + 1);
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
