// AMsg_LoginAck.cpp  -*- C++ -*-
// $Id: AMsg_LoginAck.cpp,v 1.2 1997-06-17 21:51:04-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "AMsg_LoginAck.h"
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

#include "AMsg_LoginAck.h"
#include "LyraDefs.h"
#include "AMsg.h"

#ifndef USE_INLINE
#include "AMsg_LoginAck.i"
#endif

////
// constructor
////

AMsg_LoginAck::AMsg_LoginAck()
  : LmMesg(AMsg::LOGINACK, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(DEFAULT_VERSION, LOGIN_UNUSED, 0);

  // init agent id fields
  const agent_id_t id = {_T(""), 0, 0, 0};
  for (int i=0; i<AMsg::MAX_AGENTS; i++)
	  this->SetID(i, id);
}

////
// destructor
////

AMsg_LoginAck::~AMsg_LoginAck()
{
  // empty
}

////
// Init
////

void AMsg_LoginAck::Init(int version, int status, int num_agents)
{
  SetVersion(version);
  SetStatus(status);
  SetNumAgents(num_agents);
}

////
// hton
////

void AMsg_LoginAck::hton()
{
  HTONS(data_.version);
  HTONS(data_.request_status);
  HTONS(data_.num_agents);
  for (int i = 0; i < AMsg::MAX_AGENTS; i++) {
	  HTONL(data_.ids[i].id);
	  HTONL(data_.ids[i].level_id);
	  HTONL(data_.ids[i].type);
  }
}

////
// ntoh
////

void AMsg_LoginAck::ntoh()
{
  NTOHS(data_.version);
  NTOHS(data_.request_status);
  NTOHS(data_.num_agents);
  for (int i = 0; i < AMsg::MAX_AGENTS; i++) {
	  NTOHL(data_.ids[i].id);
	  NTOHL(data_.ids[i].type);
	  NTOHL(data_.ids[i].level_id);
  }
  calc_size(); // variable-size message
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void AMsg_LoginAck::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
  fprintf(f, "<AMsg_LoginAck[%p]: ", this);
  if (ByteOrder() == ByteOrder::HOST) {
    fprintf(f, "version=%d status=%c num_agents=%d>\n", Version(), Status(), NumAgents());
  }
  else {
    fprintf(f, "(network order)>\n");
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// calc_size
////

void AMsg_LoginAck::calc_size()
{
  // initial size: entire struct, minus variable-length inventory
  int size = sizeof(data_t) - sizeof(data_.ids);
  // add inventory length
  size += (data_.num_agents * sizeof(agent_id_t));
  SetMessageSize(size);
}

