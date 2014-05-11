// GMsg_Goal.cpp  -*- C++ -*-
// $Id: GMsg_Goal.cpp,v 1.3 1997-07-29 19:32:10-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_Goal.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */
#include <stdio.h>
#include <string.h>

#include "GMsg_Goal.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_Goal.i"
#endif

////
// constructor
////

GMsg_Goal::GMsg_Goal()
  : LmMesg(GMsg::GOAL, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, UNKNOWN);
}

////
// destructor
////

GMsg_Goal::~GMsg_Goal()
{
  // empty
}

////
// Init
////

void GMsg_Goal::Init(lyra_id_t id, int messagetyp)
{
  SetID(id);
  SetRequestType(messagetyp);
}

////
// hton
////

void GMsg_Goal::hton()
{
  HTONL(data_.id);
  HTONL(data_.messagetyp);
}

////
// ntoh
////

void GMsg_Goal::ntoh()
{
  NTOHL(data_.id);
  NTOHL(data_.messagetyp);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_Goal::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_Goal[%p,%d]: "), this, sizeof(GMsg_Goal));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("id=%u request=%d>\n"), ID(), RequestType());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
