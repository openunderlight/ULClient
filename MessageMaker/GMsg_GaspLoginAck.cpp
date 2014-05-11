// GMsg_GaspLoginAck.cpp  -*- C++ -*-
// $Id: GMsg_GaspLoginAck.cpp,v 1.1 1998-01-22 15:08:27-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_GaspLoginAck.h"
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

#include "GMsg_GaspLoginAck.h"

#ifndef USE_INLINE
#include "GMsg_GaspLoginAck.i"
#endif

////
// constructor
////

GMsg_GaspLoginAck::GMsg_GaspLoginAck()
  : LmMesg(GMsg::GASPLOGINACK, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(DEFAULT_VERSION, LOGIN_UNUSED, 0);
}

////
// destructor
////

GMsg_GaspLoginAck::~GMsg_GaspLoginAck()
{
  // empty
}

////
// Init
////

void GMsg_GaspLoginAck::Init(int version, int status, int server_port)
{
  SetVersion(version);
  SetStatus(status);
  SetServerPort(server_port);
}

////
// hton
////

void GMsg_GaspLoginAck::hton()
{
  HTONL(data_.version);
  HTONL(data_.request_status);
  HTONL(data_.server_port);
}

////
// ntoh
////

void GMsg_GaspLoginAck::ntoh()
{
  NTOHL(data_.version);
  NTOHL(data_.request_status);
  NTOHL(data_.server_port);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_GaspLoginAck::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
  fprintf(f, "<GMsg_GaspLoginAck[%p,%d]: ", this, sizeof(GMsg_GaspLoginAck));
  if (ByteOrder() == ByteOrder::HOST) {
    fprintf(f, "version=%d status=%c servport=%d>\n", Version(), Status(), ServerPort());
  }
  else {
    fprintf(f, "(network order)>\n");
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
