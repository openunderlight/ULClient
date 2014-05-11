// AMsg_ControlAgent.cpp  -*- C++ -*-
// $Id: AMsg_ControlAgent.cpp,v 1.1 1997-06-17 15:34:01-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementati on

#ifdef __GNUC__
#pragma implementation "AMsg_ControlAgent.h"
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

#include "AMsg_ControlAgent.h"
#include "LyraDefs.h"
#include "AMsg.h"

#ifndef USE_INLINE
#include "AMsg_ControlAgent.i"
#endif

////
// constructor
////

AMsg_ControlAgent::AMsg_ControlAgent()
  : LmMesg(AMsg::CONTROLAGENT, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, COMMAND_UNKNOWN);
}

////
// destructor
////

AMsg_ControlAgent::~AMsg_ControlAgent()
{
  // empty
}

////
// Init
////

void AMsg_ControlAgent::Init(lyra_id_t agent_id, int command)
{
  SetAgentID(agent_id);
  SetCommand(command);
}

////
// hton
////

void AMsg_ControlAgent::hton()
{
  HTONL(data_.agent_id);
  HTONL(data_.command);
}

////
// ntoh
////

void AMsg_ControlAgent::ntoh()
{
  NTOHL(data_.agent_id);
  NTOHL(data_.command);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void AMsg_ControlAgent::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
  fprintf(f, "<AMsg_ControlAgent[%p]: ", this);
  if (ByteOrder() == ByteOrder::HOST) {
    fprintf(f, "id=%u cmd=%c>\n", AgentID(), Command());
  }
  else {
    fprintf(f, "(network order)>\n");
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
