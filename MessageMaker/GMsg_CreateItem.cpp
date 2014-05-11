// GMsg_CreateItem.cpp  -*- C++ -*-
// $Id: GMsg_CreateItem.cpp,v 1.5 1997-12-05 14:44:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_CreateItem.h"
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

#include "GMsg_CreateItem.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_CreateItem.i"
#endif

////
// constructor
////

GMsg_CreateItem::GMsg_CreateItem()
  : LmMesg(GMsg::CREATEITEM, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(LmItem::DEFAULT_INSTANCE, _T("none"));
}

////
// destructor
////

GMsg_CreateItem::~GMsg_CreateItem()
{
  // empty
}

////
// Init
////

void GMsg_CreateItem::Init(const LmItem& item, const TCHAR* description)
{
  SetItem(item);
  SetDescription(description);
}

////
// hton
////

void GMsg_CreateItem::hton()
{
  data_.item.ConvertToNetwork();
  // not converted: desc
}

////
// ntoh
////

void GMsg_CreateItem::ntoh()
{
  data_.item.ConvertToHost();
  // not converted: desc
  calc_size();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_CreateItem::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_CreateItem[%p,%d]: "), this, sizeof(GMsg_CreateItem));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("description='%s'>\n"), Description());
    Item().Dump(f, indent + 1);
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetDescription
////

void GMsg_CreateItem::SetDescription(const TCHAR* description)
{
 _tcsnccpy(data_.desc, description, sizeof(data_.desc));
  TRUNC(data_.desc, sizeof(data_.desc));
  calc_size();
}

////
// calc_size
////

void GMsg_CreateItem::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.desc);
  // only if item has a description do we count it
  if (data_.item.FlagSet(LyraItem::FLAG_HASDESCRIPTION)) {
    // add string length, plus 1 for null
    size += (_tcslen(data_.desc) + 1);
  }
  SetMessageSize(size);
}
