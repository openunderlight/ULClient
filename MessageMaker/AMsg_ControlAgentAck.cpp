// AMsg_ControlAgentAck.cpp  -*- C++ -*-
// $Id: AMsg_ControlAgentAck.cpp,v 1.1 1997-06-17 15:34:01-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "AMsg_ControlAgentAck.h"
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

#include "AMsg_ControlAgentAck.h"
#include "LyraDefs.h"
#include "AMsg.h"

#ifndef USE_INLINE
#include "AMsg_ControlAgentAck.i"
#endif

////
// constructor
////

AMsg_ControlAgentAck::AMsg_ControlAgentAck()
  : LmMesg(AMsg::CONTROLAGENTACK, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, STATUS_UNKNOWN);
}

////
// destructor
////

AMsg_ControlAgentAck::~AMsg_ControlAgentAck()
{
  // empty
}

////
// Init
////

void AMsg_ControlAgentAck::Init(lyra_id_t agent_id, int status)
{
  SetAgentID(agent_id);
  SetStatus(status);
}

////
// hton
////

void AMsg_ControlAgentAck::hton()
{
  HTONL(data_.agent_id);
  HTONL(data_.status);
}

////
// ntoh
////

void AMsg_ControlAgentAck::ntoh()
{
  NTOHL(data_.agent_id);
  NTOHL(data_.status);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void AMsg_ControlAgentAck::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
  fprintf(f, "<AMsg_ControlAgentAck[%p]: ", this);
  if (ByteOrder() == ByteOrder::HOST) {
    fprintf(f, "id=%u status=%c>\n", AgentID(), Status());
  }
  else {
    fprintf(f, "(network order)>\n");
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
