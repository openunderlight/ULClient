// GMsg_GetItem.cpp  -*- C++ -*-
// $Id: GMsg_GetItem.cpp,v 1.7 1997-07-29 19:32:10-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_GetItem.h"
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

#include "GMsg_GetItem.h"
#include "LyraDefs.h"
#include "GMsg.h"
#include "LmItemHdr.h"

#ifndef USE_INLINE
#include "GMsg_GetItem.i"
#endif

////
// constructor
////

GMsg_GetItem::GMsg_GetItem()
  : LmMesg(GMsg::GETITEM, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, LmItemHdr::DEFAULT_INSTANCE);
}

////
// destructor
////

GMsg_GetItem::~GMsg_GetItem()
{
  // empty
}

////
// Init
////

void GMsg_GetItem::Init(lyra_id_t roomid, const LmItemHdr& itemhdr)
{
  SetRoomID(roomid);
  SetItemHeader(itemhdr);
}

////
// hton
////

void GMsg_GetItem::hton()
{
  HTONL(data_.roomid);
  data_.itemhdr.ConvertToNetwork();
}

////
// ntoh
////

void GMsg_GetItem::ntoh()
{
  NTOHL(data_.roomid);
  data_.itemhdr.ConvertToHost();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_GetItem::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_GetItem[%p,%d]: "), this, sizeof(GMsg_GetItem));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("room=%u>\n"), RoomID());
    ItemHeader().Dump(f, indent + 1);
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
