// GMsg_ViewItem.cpp  -*- C++ -*-
// $Id: GMsg_ViewItem.cpp,v 1.1 1997-12-05 14:44:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_ViewItem.h"
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

#include "GMsg_ViewItem.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_ViewItem.i"
#endif

////
// constructor
////

GMsg_ViewItem::GMsg_ViewItem()
  : LmMesg(GMsg::VIEWITEM, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, LmItemHdr::DEFAULT_INSTANCE, _T("none"));
}

////
// destructor
////

GMsg_ViewItem::~GMsg_ViewItem()
{
  // empty
}

////
// Init
////

void GMsg_ViewItem::Init(lyra_id_t sourceid, const LmItemHdr& itemheader, const TCHAR* itemname)
{
  SetSourceID(sourceid);
  SetItemHeader(itemheader);
  SetItemName(itemname);
}

////
// hton
////

void GMsg_ViewItem::hton()
{
  HTONL(data_.sourceid);
  data_.itemheader.ConvertToNetwork();
  // not converted: ItemName
}

////
// ntoh
////

void GMsg_ViewItem::ntoh()
{
  NTOHL(data_.sourceid);
  data_.itemheader.ConvertToHost();
  // not converted: ItemName
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_ViewItem::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_ViewItem[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("source=%u name='%s'>\n"), SourceID(), ItemName());
    ItemHeader().Dump(f, indent + 1);
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetItemName
////

void GMsg_ViewItem::SetItemName(const TCHAR* itemname)
{
 _tcsnccpy(data_.itemname, itemname, sizeof(data_.itemname));
  TRUNC(data_.itemname, sizeof(data_.itemname));
}
