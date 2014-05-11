// RMsg_Ping.cpp  -*- C++ -*-
// $Id: RMsg_Ping.cpp,v 1.3 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_Ping.h"
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

#include "RMsg_Ping.h"
#include "LyraDefs.h"
#include "RMsg.h"

#ifndef USE_INLINE
#include "RMsg_Ping.i"
#endif

////
// constructor
////

RMsg_Ping::RMsg_Ping()
  : LmMesg(RMsg::PING, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0);
}

////
// destructor
////

RMsg_Ping::~RMsg_Ping()
{
  // empty
}

////
// Init
////

void RMsg_Ping::Init(int nonce)
{
  SetNonce(nonce);
}

////
// hton
////

void RMsg_Ping::hton()
{
  HTONL(data_.nonce);
}

////
// ntoh
////

void RMsg_Ping::ntoh()
{
  NTOHL(data_.nonce);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_Ping::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_Ping[%p,%d]: "), this, sizeof(RMsg_Ping));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("nonce=%d>\n"), Nonce());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
