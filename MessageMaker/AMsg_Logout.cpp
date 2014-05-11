// AMsg_Logout.cpp  -*- C++ -*-
// $Id: AMsg_Logout.cpp,v 1.1 1997-06-17 15:34:01-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "AMsg_Logout.h"
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

#include "AMsg_Logout.h"
#include "LyraDefs.h"
#include "AMsg.h"

#ifndef USE_INLINE
#include "AMsg_Logout.i"
#endif

////
// constructor
////

AMsg_Logout::AMsg_Logout()
  : LmMesg(AMsg::LOGOUT, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  //Init();
}

////
// destructor
////

AMsg_Logout::~AMsg_Logout()
{
  // empty
}

////
// Init
////

void AMsg_Logout::Init()
{
  // empty
}

////
// hton
////

void AMsg_Logout::hton()
{
  // empty
}

////
// ntoh
////

void AMsg_Logout::ntoh()
{
  // empty
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void AMsg_Logout::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
  fprintf(f, "<AMsg_Logout[%p]: ", this);
  if (ByteOrder() == ByteOrder::HOST) {
    fprintf(f, "(no data)>\n");
  }
  else {
    fprintf(f, "(network order)>\n");
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
