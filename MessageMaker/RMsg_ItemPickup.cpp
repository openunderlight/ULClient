// RMsg_ItemPickup.cpp  -*- C++ -*-
// $Id: RMsg_ItemPickup.cpp,v 1.13 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_ItemPickup.h"
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

#include "RMsg_ItemPickup.h"
#include "LyraDefs.h"
#include "RMsg.h"
#include "LmItemHdr.h"

#ifndef USE_INLINE
#include "RMsg_ItemPickup.i"
#endif

////
// constructor
////

RMsg_ItemPickup::RMsg_ItemPickup()
  : LmMesg(RMsg::ITEMPICKUP, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(DEFAULT_NUMITEMS);
  for (int i = 0; i < MAX_ITEMS; ++i) {
    SetItemHeader(i, LmItemHdr::DEFAULT_INSTANCE);
  }
}

////
// destructor
////

RMsg_ItemPickup::~RMsg_ItemPickup()
{
  // empty
}

////
// Init
////

void RMsg_ItemPickup::Init(int num_items)
{
  SetNumItems(num_items);
}

////
// hton
////

void RMsg_ItemPickup::hton()
{
  for (int i = 0; i < NumItems(); ++i) {
    data_.itemhdrs[i].ConvertToNetwork();
  }
}

////
// ntoh
////

void RMsg_ItemPickup::ntoh()
{
  calc_items(); // calculate number of items, calls calc_size()
  for (int i = 0; i < NumItems(); ++i) {
    data_.itemhdrs[i].ConvertToHost();
  }
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_ItemPickup::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_ItemPickup[%p,%d]: "), this, sizeof(RMsg_ItemPickup));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("items=%d>\n"), NumItems());
    for (int i = 0; i < NumItems(); ++i) {
      ItemHeader(i).Dump(f, indent + 1);
    }
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetNumItems
////

void RMsg_ItemPickup::SetNumItems(int num)
{
  // avoid overflow
  num_items_ = MIN(num, MAX_ITEMS);
  calc_size();
}

////
// calc_size - calculate message size based on number of items
////

void RMsg_ItemPickup::calc_size()
{
  // initial size: overall size minus variable-length field
  int size = sizeof(data_t) - sizeof(data_.itemhdrs);
  // add space for items
  size += NumItems() * sizeof(LmItemHdr);
  SetMessageSize(size);
}

////
// calc_items - calculate number of items based on current message size
////

void RMsg_ItemPickup::calc_items()
{
  int msgsize = MessageSize();
  // determine portion of message size related to fixed fields
  int fixed_size = sizeof(data_t) - sizeof(data_.itemhdrs);
  // subtract this from the overall message size; result is the
  // size attributable to the variable-sized field
  msgsize -= fixed_size;
  // calculate number of items
  SetNumItems(msgsize / sizeof(LmItemHdr));
}
