// GMsg_DestroyItem.cpp  -*- C++ -*-
// $Id: GMsg_DestroyItem.cpp,v 1.4 1997-07-29 19:32:10-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_DestroyItem.h"
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

#include "GMsg_DestroyItem.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_DestroyItem.i"
#endif

////
// constructor
////

GMsg_DestroyItem::GMsg_DestroyItem()
  : LmMesg(GMsg::DESTROYITEM, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(LmItemHdr::DEFAULT_INSTANCE);
}

////
// destructor
////

GMsg_DestroyItem::~GMsg_DestroyItem()
{
  // empty
}

////
// Init
////

void GMsg_DestroyItem::Init(const LmItemHdr& itemhdr)
{
  SetItemHeader(itemhdr);
}

////
// hton
////

void GMsg_DestroyItem::hton()
{
  data_.itemhdr.ConvertToNetwork();
}

////
// ntoh
////

void GMsg_DestroyItem::ntoh()
{
  data_.itemhdr.ConvertToHost();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_DestroyItem::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_DestroyItem[%p,%d]: "), this, sizeof(GMsg_DestroyItem));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T(">\n"));
    ItemHeader().Dump(f, indent + 1);
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
