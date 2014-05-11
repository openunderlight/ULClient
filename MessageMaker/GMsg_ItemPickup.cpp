// GMsg_ItemPickup.cpp  -*- C++ -*-
// $Id: GMsg_ItemPickup.cpp,v 1.6 1997-07-29 19:32:10-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_ItemPickup.h"
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

#include "GMsg_ItemPickup.h"
#include "LyraDefs.h"
#include "GMsg.h"
#include "LmItem.h"

#ifndef USE_INLINE
#include "GMsg_ItemPickup.i"
#endif

////
// constructor
////

GMsg_ItemPickup::GMsg_ItemPickup()
  : LmMesg(GMsg::ITEMPICKUP, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(PICKUP_UNKNOWN, LmItem::DEFAULT_INSTANCE);
}

////
// destructor
////

GMsg_ItemPickup::~GMsg_ItemPickup()
{
  // empty
}

////
// Init
////

void GMsg_ItemPickup::Init(int status, const LmItem& item)
{
  SetStatus(status);
  SetItem(item);
}

////
// hton
////

void GMsg_ItemPickup::hton()
{
  HTONL(data_.status);
  data_.item.ConvertToNetwork();
}

////
// ntoh
////

void GMsg_ItemPickup::ntoh()
{
  NTOHL(data_.status);
  data_.item.ConvertToHost();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_ItemPickup::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_ItemPickup[%p,%d]: "), this, sizeof(GMsg_ItemPickup));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("status=%c>\n"), Status());
    Item().Dump(f, indent + 1);
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
