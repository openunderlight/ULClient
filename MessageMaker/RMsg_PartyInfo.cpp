// RMsg_PartyInfo.cpp  -*- C++ -*-
// $Id: RMsg_PartyInfo.cpp,v 1.12 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_PartyInfo.h"
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

#include "RMsg_PartyInfo.h"
#include "LyraDefs.h"
#include "RMsg.h"
#include "RmRemotePlayer.h"

#ifndef USE_INLINE
#include "RMsg_PartyInfo.i"
#endif

////
// constructor
////

RMsg_PartyInfo::RMsg_PartyInfo()
  : LmMesg(RMsg::PARTYINFO, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, DEFAULT_PARTYSIZE);
  for (int i = 0; i < Lyra::MAX_PARTYSIZE; ++i) {
    SetPartyMember(i, RmRemotePlayer::DEFAULT_INSTANCE);
  }
}

////
// destructor
////

RMsg_PartyInfo::~RMsg_PartyInfo()
{
  // empty
}

////
// Init
////

void RMsg_PartyInfo::Init(lyra_id_t leaderid, int psize)
{
  SetLeaderID(leaderid);
  SetPartySize(psize);
}

////
// hton
////

void RMsg_PartyInfo::hton()
{
  HTONL(data_.leaderid);
  for (int i = 0; i < PartySize(); ++i) {
    data_.members[i].ConvertToNetwork();
  }
}

////
// ntoh
////

void RMsg_PartyInfo::ntoh()
{
  calc_players(); // calculate party size, calls calc_size()
  NTOHL(data_.leaderid);
  for (int i = 0; i < PartySize(); ++i) {
    data_.members[i].ConvertToHost();
  }
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_PartyInfo::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_PartyInfo[%p,%d]: "), this, sizeof(RMsg_PartyInfo));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("leader=%u size=%d>\n"), LeaderID(), PartySize());
    for (int i = 0; i < PartySize(); ++i) {
      PartyMember(i).Dump(f, indent + 1);
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
// SetPartySize
////

void RMsg_PartyInfo::SetPartySize(int num)
{
  // prevent overflow
  num_members_ = MIN(num, Lyra::MAX_PARTYSIZE);
  calc_size();
}

////
// calc_size - calculate message size based on party size
////

void RMsg_PartyInfo::calc_size()
{
  // initial size: overall size minus variable-length field
  int size = sizeof(data_t) - sizeof(data_.members);
  // add space for players
  size += PartySize() * sizeof(RmRemotePlayer);
  SetMessageSize(size);
}

////
// calc_players - calculate number of players based on current message size
////

void RMsg_PartyInfo::calc_players()
{
  int msgsize = MessageSize();
  // determine portion of message size related to fixed fields
  int fixed_size = sizeof(data_t) - sizeof(data_.members);
  // subtract this from the overall message size; result is the
  // size attributable to the variable-sized field
  msgsize -= fixed_size;
  // calculate number of items
  SetPartySize(msgsize / sizeof(RmRemotePlayer));
}
