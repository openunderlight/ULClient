// AMsg_AgentInfo.cpp  -*- C++ -*-
// $Id: AMsg_AgentInfo.cpp,v 1.1 1997-06-17 15:34:01-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "AMsg_AgentInfo.h"
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

#include "AMsg_AgentInfo.h"
#include "LyraDefs.h"
#include "AMsg.h"

#ifndef USE_INLINE
#include "AMsg_AgentInfo.i"
#endif

////
// constructor
////

AMsg_AgentInfo::AMsg_AgentInfo()
  : LmMesg(AMsg::AGENTINFO, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, 0, 0, 0, STATUS_NONE, 0, 0, 0, 0);
}

////
// destructor
////

AMsg_AgentInfo::~AMsg_AgentInfo()
{
  // empty
}

////
// Init
////

void AMsg_AgentInfo::Init(lyra_id_t agent_id, short kills, 
 		  short deaths, unsigned char frame_rate, unsigned char status,
		  unsigned char busy, unsigned char room_id, short x, short y)
{
  SetAgentID(agent_id);
  SetKills(kills);
  SetDeaths(deaths);
  SetFrameRate(frame_rate);
  SetStatus(status);
  SetBusy(busy);
  SetRoomID(room_id);
  SetX(x);
  SetY(y);
}

////
// hton
////

void AMsg_AgentInfo::hton()
{
  HTONL(data_.agent_id);
  HTONS(data_.deaths);
  HTONS(data_.kills);
  HTONS(data_.x);
  HTONS(data_.y);
}

////
// ntoh
////

void AMsg_AgentInfo::ntoh()
{
  NTOHL(data_.agent_id);
  NTOHS(data_.deaths);
  NTOHS(data_.kills);
  NTOHS(data_.x);
  NTOHS(data_.y);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void AMsg_AgentInfo::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
  fprintf(f, "<AMsg_AgentInfo[%p]: ", this);
  if (ByteOrder() == ByteOrder::HOST) {
    fprintf(f, "id=%u deaths=%d kills=%d frame=%d status=%d busy=%d room_id=%d x=%d y=%d>\n",
	    AgentID(), Deaths(), Kills(), FrameRate(), Status(), Busy(), RoomID(), X(), Y());
  }
  else {
    fprintf(f, "(network order)>\n");
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
