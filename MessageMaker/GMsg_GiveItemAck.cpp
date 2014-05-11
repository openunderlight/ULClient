// GMsg_GiveItemAck.cpp  -*- C++ -*-
// $Id: GMsg_GiveItemAck.cpp,v 1.1 1997-09-19 16:42:16-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_GiveItemAck.h"
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

#include "GMsg_GiveItemAck.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_GiveItemAck.i"
#endif

////
// constructor
////

GMsg_GiveItemAck::GMsg_GiveItemAck()
  : LmMesg(GMsg::GIVEITEMACK, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0, LmItemHdr::DEFAULT_INSTANCE);
}

////
// destructor
////

GMsg_GiveItemAck::~GMsg_GiveItemAck()
{
  // empty
}

////
// Init
////

void GMsg_GiveItemAck::Init(int status, const LmItemHdr& itemheader)
{
  SetStatus(status);
  SetItemHeader(itemheader);
}

////
// hton
////

void GMsg_GiveItemAck::hton()
{
  HTONL(data_.status);
  data_.itemheader.ConvertToNetwork();
}

////
// ntoh
////

void GMsg_GiveItemAck::ntoh()
{
  NTOHL(data_.status);
  data_.itemheader.ConvertToHost();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_GiveItemAck::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_GiveItemAck[%p]: "), this);
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
