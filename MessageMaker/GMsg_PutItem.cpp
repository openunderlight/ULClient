// GMsg_PutItem.cpp  -*- C++ -*-
// $Id: GMsg_PutItem.cpp,v 1.8 1997-07-29 19:32:10-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_PutItem.h"
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

#include "GMsg_PutItem.h"
#include "LyraDefs.h"
#include "GMsg.h"
#include "LmItemHdr.h"
#include "LmPosition.h"

#ifndef USE_INLINE
#include "GMsg_PutItem.i"
#endif

////
// constructor
////

GMsg_PutItem::GMsg_PutItem()
  : LmMesg(GMsg::PUTITEM, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, LmItemHdr::DEFAULT_INSTANCE, LmPosition::DEFAULT_INSTANCE, DEFAULT_TTL);
}

////
// destructor
////

GMsg_PutItem::~GMsg_PutItem()
{
  // empty
}

////
// Init
////

void GMsg_PutItem::Init(lyra_id_t roomid, const LmItemHdr& itemhdr, const LmPosition& position,
			int ttl)
{
  SetRoomID(roomid);
  SetTTL(ttl);
  SetItemHeader(itemhdr);
  SetPosition(position);
}

////
// hton
////

void GMsg_PutItem::hton()
{
  HTONL(data_.roomid);
  HTONL(data_.ttl);
  data_.itemhdr.ConvertToNetwork();
  data_.position.ConvertToNetwork();
}

////
// ntoh
////

void GMsg_PutItem::ntoh()
{
  NTOHL(data_.roomid);
  NTOHL(data_.ttl);
  data_.itemhdr.ConvertToHost();
  data_.position.ConvertToHost();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_PutItem::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_PutItem[%p,%d]: "), this, sizeof(GMsg_PutItem));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("room=%u ttl=%d>\n"), RoomID(), TTL());
    Position().Dump(f, indent + 1);
    ItemHeader().Dump(f, indent + 1);
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
