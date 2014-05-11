// RMsg_ItemDrop.cpp  -*- C++ -*-
// $Id: RMsg_ItemDrop.cpp,v 1.15 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_ItemDrop.h"
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

#include "RMsg_ItemDrop.h"
#include "LyraDefs.h"
#include "RMsg.h"

#ifndef USE_INLINE
#include "RMsg_ItemDrop.i"
#endif

////
// constructor
////

RMsg_ItemDrop::RMsg_ItemDrop()
  : LmMesg(RMsg::ITEMDROP, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(DEFAULT_NUMITEMS);
  for (int i = 0; i < MAX_ITEMS; ++i) {
    SetItem(i, LmItem::DEFAULT_INSTANCE, LmPosition::DEFAULT_INSTANCE);
  }
}

////
// destructor
////

RMsg_ItemDrop::~RMsg_ItemDrop()
{
  // empty
}

////
// Init
////

void RMsg_ItemDrop::Init(int num_items)
{
  SetNumItems(num_items);
}

////
// hton
////

void RMsg_ItemDrop::hton()
{
  for (int i = 0; i < NumItems(); ++i) {
    data_.items[i].item.ConvertToNetwork();
    data_.items[i].pos.ConvertToNetwork();
  }
}

////
// ntoh
////

void RMsg_ItemDrop::ntoh()
{
  calc_items(); // compute number of items, calls calc_size()
  for (int i = 0; i < NumItems(); ++i) {
    data_.items[i].item.ConvertToHost();
    data_.items[i].pos.ConvertToHost();
  }
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_ItemDrop::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_ItemDrop[%p,%d]: "), this, sizeof(RMsg_ItemDrop));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("items=%d>\n"), NumItems());
    for (int i = 0; i < NumItems(); ++i) {
      Item(i).Dump(f, indent + 1);
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

void RMsg_ItemDrop::SetNumItems(int num)
{
  // avoid overflow
  num_items_ = MIN(num, MAX_ITEMS);
  calc_size();
}

////
// calc_size - calculate message size based on number of items
////

void RMsg_ItemDrop::calc_size()
{
  // initial size: overall size minus variable-length field
  int size = sizeof(data_t) - sizeof(data_.items);
  // add space for items
  size += NumItems() * sizeof(full_item_t);
  SetMessageSize(size);
}

////
// calc_items - calculate number of items based on current message size
////

void RMsg_ItemDrop::calc_items()
{
  int msgsize = MessageSize();
  // determine portion of message size related to fixed fields
  int fixed_size = sizeof(data_t) - sizeof(data_.items);
  // subtract this from the overall message size; result is the
  // size attributable to the variable-sized field
  msgsize -= fixed_size;
  // calculate number of items
  SetNumItems(msgsize / sizeof(full_item_t));
}
