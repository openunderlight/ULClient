// GMsg_TakeItem.cpp  -*- C++ -*-
// $Id: GMsg_TakeItem.cpp,v 1.1 1997-09-18 16:51:12-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_TakeItem.h"
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

#include "GMsg_TakeItem.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_TakeItem.i"
#endif

////
// constructor
////

GMsg_TakeItem::GMsg_TakeItem()
  : LmMesg(GMsg::TAKEITEM, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0, LmItem::DEFAULT_INSTANCE);
}

////
// destructor
////

GMsg_TakeItem::~GMsg_TakeItem()
{
  // empty
}

////
// Init
////

void GMsg_TakeItem::Init(lyra_id_t sourceid, const LmItem& item)
{
  SetSourceID(sourceid);
  SetItem(item);
}

////
// hton
////

void GMsg_TakeItem::hton()
{
  HTONL(data_.sourceid);
  data_.item.ConvertToNetwork();
}

////
// ntoh
////

void GMsg_TakeItem::ntoh()
{
  NTOHL(data_.sourceid);
  data_.item.ConvertToHost();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_TakeItem::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_TakeItem[%p]: "), this);
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
