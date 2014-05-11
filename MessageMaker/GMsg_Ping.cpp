// GMsg_Ping.cpp  -*- C++ -*-
// $Id: GMsg_Ping.cpp,v 1.4 1997-10-22 19:21:08-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_Ping.h"
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

#include "GMsg_Ping.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_Ping.i"
#endif

////
// constructor
////

GMsg_Ping::GMsg_Ping()
  : LmMesg(GMsg::PING, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0);
}

////
// destructor
////

GMsg_Ping::~GMsg_Ping()
{
  // empty
}

////
// Init
////

void GMsg_Ping::Init(int nonce, int ping_type)
{
  SetNonce(nonce);
  SetPingType(ping_type);
}

////
// hton
////

void GMsg_Ping::hton()
{
  HTONL(data_.nonce);
  HTONL(data_.ping_type);
}

////
// ntoh
////

void GMsg_Ping::ntoh()
{
  NTOHL(data_.nonce);
  NTOHL(data_.ping_type);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_Ping::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_Ping[%p,%d]: "), this, sizeof(GMsg_Ping));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("nonce=%d type=%d>\n"), Nonce(), PingType());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
