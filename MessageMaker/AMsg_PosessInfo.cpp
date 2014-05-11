// AMsg_PosessInfo.cpp  -*- C++ -*-
// $Id: AMsg_PosessInfo.cpp,v 1.1 1997-06-18 15:39:42-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "AMsg_PosessInfo.h"
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

#include "AMsg_PosessInfo.h"
#include "LyraDefs.h"
#include "AMsg.h"

#ifndef USE_INLINE
#include "AMsg_PosessInfo.i"
#endif

////
// constructor
////

AMsg_PosessInfo::AMsg_PosessInfo()
  : LmMesg(AMsg::POSESSINFO, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(FAILED);
}

////
// destructor
////

AMsg_PosessInfo::~AMsg_PosessInfo()
{
  // empty
}

////
// Init
////

void AMsg_PosessInfo::Init(int status)
{
   SetStatus(status);
}

////
// hton
////

void AMsg_PosessInfo::hton()
{
	HTONL(data_.status);
  // no conversion for strings
}

////
// ntoh
////

void AMsg_PosessInfo::ntoh()
{
	NTOHL(data_.status);
  // no conversion for strings
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void AMsg_PosessInfo::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
  fprintf(f, "<AMsg_PosessInfo[%p]: ", this);
#ifndef WIN32 // breaks client
  if (ByteOrder() == ByteOrder::HOST) {
    _ftprintf(f, "type=%d u='%s' p ='%s'>\n", this->Status(), this->Name(), this->Password());
  }
  else {
    _ftprintf(f, "(network order)>\n");
  }
#endif
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */




