// GMsg_ItemDrop.cpp  -*- C++ -*-
// $Id: GMsg_ItemDrop.cpp,v 1.7 1997-07-29 19:32:10-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_ItemDrop.h"
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

#include "GMsg_ItemDrop.h"
#include "LyraDefs.h"
#include "GMsg.h"
#include "LmItemHdr.h"

#ifndef USE_INLINE
#include "GMsg_ItemDrop.i"
#endif

////
// constructor
////

GMsg_ItemDrop::GMsg_ItemDrop()
  : LmMesg(GMsg::ITEMDROP, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(DROP_UNKNOWN, LmItemHdr::DEFAULT_INSTANCE);
}

////
// destructor
////

GMsg_ItemDrop::~GMsg_ItemDrop()
{
  // empty
}

////
// Init
////

void GMsg_ItemDrop::Init(int status, const LmItemHdr& itemhdr)
{
  SetStatus(status);
  SetItemHeader(itemhdr);
}

////
// hton
////

void GMsg_ItemDrop::hton()
{
  HTONL(data_.status);
  data_.itemhdr.ConvertToNetwork();
}

////
// ntoh
////

void GMsg_ItemDrop::ntoh()
{
  NTOHL(data_.status);
  data_.itemhdr.ConvertToHost();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_ItemDrop::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_ItemDrop[%p,%d]: "), this, sizeof(GMsg_ItemDrop));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("status=%c>\n"), Status());
    ItemHeader().Dump(f, indent + 1);
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
