// GMsg_Logout.cpp  -*- C++ -*-
// $Id: GMsg_Logout.cpp,v 1.9 1997-09-25 16:26:38-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_Logout.h"
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

#include "GMsg_Logout.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_Logout.i"
#endif

////
// constructor
////

GMsg_Logout::GMsg_Logout()
  : LmMesg(GMsg::LOGOUT, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(LOGOUT_NORMAL, 0);
}

////
// destructor
////

GMsg_Logout::~GMsg_Logout()
{
  // empty
}

////
// Init
////

void GMsg_Logout::Init(int status, int num_items)
{
  SetStatus(status);
  SetNumItems(num_items);
}

////
// hton
////

void GMsg_Logout::hton()
{
  HTONS(data_.status);
  for (int i = 0; i < NumItems(); ++i) {
    HTONL(data_.items[i].serial);
    HTONS(data_.items[i].pos);
    HTONS(data_.items[i].flags);
  }
  HTONS(data_.num_items);
}

////
// ntoh
////

void GMsg_Logout::ntoh()
{
  NTOHS(data_.status);
  NTOHS(data_.num_items);
  for (int i = 0; i < NumItems(); ++i) {
    NTOHL(data_.items[i].serial);
    NTOHS(data_.items[i].pos);
    NTOHS(data_.items[i].flags);
  }
  calc_size(); // variable-length message
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_Logout::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_Logout[%p,%d]: "), this, sizeof(GMsg_Logout));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("status=%c items=%d>\n"), Status(), NumItems());
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

void GMsg_Logout::SetNumItems(int num_items)
{
  // prevent overflow
  data_.num_items = MIN(num_items, Lyra::INVENTORY_MAX);
  calc_size();
}

////
// calc_size
////

void GMsg_Logout::calc_size()
{
  // initial size: entire struct, minus variable-length inventory
  int size = sizeof(data_t) - sizeof(data_.items);
  // add inventory length
  size += (data_.num_items * sizeof(item_state_t));
  SetMessageSize(size);
}
