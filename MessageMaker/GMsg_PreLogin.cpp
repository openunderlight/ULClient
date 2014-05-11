// GMsg_PreLogin.cpp  -*- C++ -*-
// $Id: GMsg_PreLogin.cpp,v 1.12 1998-04-17 16:37:44-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_PreLogin.h"
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

#include "GMsg_PreLogin.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_PreLogin.i"
#endif

////
// constructor
////

GMsg_PreLogin::GMsg_PreLogin()
  : LmMesg(GMsg::PRELOGIN, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(DEFAULT_VERSION);
}

////
// destructor
////

GMsg_PreLogin::~GMsg_PreLogin()
{
  // empty
}

////
// Init
////

void GMsg_PreLogin::Init(int version) 
{
  SetVersion(version);
}

////
// hton
////

void GMsg_PreLogin::hton()
{
  HTONL(data_.version);
}

////
// ntoh
////

void GMsg_PreLogin::ntoh()
{
  NTOHL(data_.version);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_PreLogin::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_PreLogin[%p,%d]: "), this, sizeof(GMsg_PreLogin));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("version=%d>\n"), Version());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */


