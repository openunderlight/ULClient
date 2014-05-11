// GMsg_GaspLogin.cpp  -*- C++ -*-
// $Id: GMsg_GaspLogin.cpp,v 1.1 1998-01-22 15:08:27-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_GaspLogin.h"
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

#include "GMsg_GaspLogin.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_GaspLogin.i"
#endif

////
// constructor
////

GMsg_GaspLogin::GMsg_GaspLogin()
  : LmMesg(GMsg::GASPLOGIN, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(DEFAULT_VERSION, 0, 0);
}

////
// destructor
////

GMsg_GaspLogin::~GMsg_GaspLogin()
{
  // empty
}

////
// Init
////

void GMsg_GaspLogin::Init(int version, mpath_id_t mpathid, int subversion)
{
  SetVersion(version);
  SetSubVersion(subversion);
  SetMPathID(mpathid);
}

////
// hton
////

void GMsg_GaspLogin::hton()
{
  HTONL(data_.version);
  HTONL(data_.subversion);
  HTONL(data_.mpathid);
}

////
// ntoh
////

void GMsg_GaspLogin::ntoh()
{
  NTOHL(data_.version);
  NTOHL(data_.subversion);
  NTOHL(data_.mpathid);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_GaspLogin::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
  fprintf(f, "<GMsg_GaspLogin[%p,%d]: ", this, sizeof(GMsg_GaspLogin));
  if (ByteOrder() == ByteOrder::HOST) {
    fprintf(f, "mpathid=%u version=%d/%d>\n", MPathID(), Version(), SubVersion());
  }
  else {
    fprintf(f, "(network order)>\n");
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
