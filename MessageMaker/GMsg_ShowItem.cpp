// GMsg_ShowItem.cpp  -*- C++ -*-
// $Id: GMsg_ShowItem.cpp,v 1.2 1997-12-11 16:25:07-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_ShowItem.h"
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

#include "GMsg_ShowItem.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_ShowItem.i"
#endif

////
// constructor
////

GMsg_ShowItem::GMsg_ShowItem()
  : LmMesg(GMsg::SHOWITEM, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, LmItemHdr::DEFAULT_INSTANCE);
}

////
// destructor
////

GMsg_ShowItem::~GMsg_ShowItem()
{
  // empty
}

////
// Init
////

void GMsg_ShowItem::Init(lyra_id_t targetid, const LmItemHdr& itemheader)
{
  SetTargetID(targetid);
  SetItemHeader(itemheader);
}

////
// hton
////

void GMsg_ShowItem::hton()
{
  HTONL(data_.targetid);
  data_.itemheader.ConvertToNetwork();
}

////
// ntoh
////

void GMsg_ShowItem::ntoh()
{
  NTOHL(data_.targetid);
  data_.itemheader.ConvertToHost();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_ShowItem::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_ShowItem[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("target=%u>\n"), TargetID());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
