// RMsg_ItemHdrDrop.cpp  -*- C++ -*-
// $Id: RMsg_ItemHdrDrop.cpp,v 1.10 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_ItemHdrDrop.h"
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

#include "RMsg_ItemHdrDrop.h"
#include "LyraDefs.h"
#include "RMsg.h"

#ifndef USE_INLINE
#include "RMsg_ItemHdrDrop.i"
#endif

////
// constructor
////

RMsg_ItemHdrDrop::RMsg_ItemHdrDrop()
  : LmMesg(RMsg::ITEMHDRDROP, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(DEFAULT_NUMITEMS);
  for (int i = 0; i < MAX_ITEMS; ++i) {
    SetItemHeader(i, LmItemHdr::DEFAULT_INSTANCE, LmPosition::DEFAULT_INSTANCE);
  }
}

////
// destructor
////

RMsg_ItemHdrDrop::~RMsg_ItemHdrDrop()
{
  // empty
}

////
// Init
////

void RMsg_ItemHdrDrop::Init(int num_items)
{
  SetNumItems(num_items);
}

////
// hton
////

void RMsg_ItemHdrDrop::hton()
{
  for (int i = 0; i < NumItems(); ++i) {
    data_.itemhdrs[i].itemhdr.ConvertToNetwork();
    data_.itemhdrs[i].pos.ConvertToNetwork();
  }
}

////
// ntoh
////

void RMsg_ItemHdrDrop::ntoh()
{
  calc_items(); // calculate number of items, calls calc_size()
  for (int i = 0; i < NumItems(); ++i) {
    data_.itemhdrs[i].itemhdr.ConvertToHost();
    data_.itemhdrs[i].pos.ConvertToHost();
  }
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_ItemHdrDrop::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_ItemHdrDrop[%p,%d]: "), this, sizeof(RMsg_ItemHdrDrop));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("items=%d>\n"), NumItems());
    for (int i = 0; i < NumItems(); ++i) {
      ItemHeader(i).Dump(f, indent + 1);
      Position(i).Dump(f, indent + 2);
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

void RMsg_ItemHdrDrop::SetNumItems(int num_items)
{
  // avoid overflow
  num_items_ = MIN(num_items, MAX_ITEMS);
  // calculcate and assign message size
  calc_size();
}

////
// calc_size - calculate message size based on current number of items
////

void RMsg_ItemHdrDrop::calc_size()
{
  // initial size: overall size minus variable-length field
  int size = sizeof(data_t) - sizeof(data_.itemhdrs);
  // add space for items
  size += NumItems() * sizeof(header_item_t);
  SetMessageSize(size);
}

////
// calc_items - calculate number of items based on current message size
////

void RMsg_ItemHdrDrop::calc_items()
{
  int msgsize = MessageSize();
  // determine portion of message size related to fixed fields
  int fixed_size = sizeof(data_t) - sizeof(data_.itemhdrs);
  // subtract this from the overall message size; result is the
  // size attributable to the variable-sized field
  msgsize -= fixed_size;
  // calculate number of items
  SetNumItems(msgsize / sizeof(header_item_t));
}
