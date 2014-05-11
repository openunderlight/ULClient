// GMsg_PreLoginAck.cpp  -*- C++ -*-
// $Id: GMsg_PreLoginAck.cpp,v 1.19 1997-10-01 17:53:07-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_PreLoginAck.h"
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

#include "GMsg_PreLoginAck.h"

#ifndef USE_INLINE
#include "GMsg_PreLoginAck.i"
#endif

////
// constructor
////

GMsg_PreLoginAck::GMsg_PreLoginAck()
  : LmMesg(GMsg::PRELOGINACK, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  SetChallenge(_T("none"));
}

////
// destructor
////

GMsg_PreLoginAck::~GMsg_PreLoginAck()
{
  // empty
}

////
// Init
////

void GMsg_PreLoginAck::Init(int version, int status, const TCHAR* challenge)
{
  SetVersion(version);
  SetStatus(status);
  SetChallenge(challenge);
}

////
// hton
////

void GMsg_PreLoginAck::hton()
{
  HTONS(data_.version);
  HTONS(data_.status);
}

////
// ntoh
////

void GMsg_PreLoginAck::ntoh()
{
  NTOHL(data_.version);
  NTOHL(data_.status);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_PreLoginAck::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_PreLoginAck[%p,%d]: "), this, sizeof(GMsg_PreLoginAck));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("version=%d status=%dchallenge=%s>\n"),
	    Version(), Status(), Challenge());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */


////
// SetChallenge
////

void GMsg_PreLoginAck::SetChallenge(const TCHAR* Challenge)
{
  _tcsnccpy(data_.challenge, Challenge, Lyra::CHALLENGE_LENGTH);
  TRUNC(data_.challenge, Lyra::CHALLENGE_LENGTH);
}
