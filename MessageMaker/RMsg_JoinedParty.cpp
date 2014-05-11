// RMsg_JoinedParty.cpp  -*- C++ -*-
// $Id: RMsg_JoinedParty.cpp,v 1.8 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_JoinedParty.h"
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

#include "RMsg_JoinedParty.h"
#include "LyraDefs.h"
#include "RMsg.h"
#include "RmRemotePlayer.h"

#ifndef USE_INLINE
#include "RMsg_JoinedParty.i"
#endif

////
// constructor
////

RMsg_JoinedParty::RMsg_JoinedParty()
  : LmMesg(RMsg::JOINEDPARTY, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(RmRemotePlayer::DEFAULT_INSTANCE);
}

////
// destructor
////

RMsg_JoinedParty::~RMsg_JoinedParty()
{
  // empty
}

////
// Init
////

void RMsg_JoinedParty::Init(const RmRemotePlayer& player)
{
  SetPartyMember(player);
}

////
// hton
////

void RMsg_JoinedParty::hton()
{
  data_.member.ConvertToNetwork();
}

////
// ntoh
////

void RMsg_JoinedParty::ntoh()
{
  data_.member.ConvertToHost();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_JoinedParty::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_JoinedParty[%p,%d]: "), this, sizeof(RMsg_JoinedParty));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T(">\n"));
    PartyMember().Dump(f, indent + 1);
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
