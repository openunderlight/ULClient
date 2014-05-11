// RMsg_Update.cpp  -*- C++ -*-
// $Id: RMsg_Update.cpp,v 1.5 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "RMsg_Update.h"
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

#include "RMsg_Update.h"
#include "RMsg.h"
#include "LyraDefs.h"

#ifndef USE_INLINE
#include "RMsg_Update.i"
#endif

////
// constructor
////

RMsg_Update::RMsg_Update()
  : LmMesg(RMsg::UPDATE, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, LmPeerUpdate::DEFAULT_INSTANCE);
}

////
// destructor
////

RMsg_Update::~RMsg_Update()
{
  // empty
}

////
// Init
////

void RMsg_Update::Init(lyra_id_t playerid, const LmPeerUpdate& update)
{
  SetPeerUpdate(update);
  // do this after, since the update structure overwrites the playerid
  // (this is sort of redundant)
  SetPlayerID(playerid);
}

////
// hton
////

void RMsg_Update::hton()
{
  data_.update.ConvertToNetwork();
}

////
// ntoh
////

void RMsg_Update::ntoh()
{
  data_.update.ConvertToHost();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_Update::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_Update[%p,%d]: "), this, sizeof(RMsg_Update));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T(">\n"));
    PeerUpdate().Dump(f, indent + 1);
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
