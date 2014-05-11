// RMsg_GetAvatarDescription.cpp  -*- C++ -*-
// $Id: RMsg_GetAvatarDescription.cpp,v 1.1 1998-04-17 16:27:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_GetAvatarDescription.h"
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

#include "RMsg_GetAvatarDescription.h"
#include "LyraDefs.h"
#include "RMsg.h"

#ifndef USE_INLINE
#include "RMsg_GetAvatarDescription.i"
#endif

////
// constructor
////

RMsg_GetAvatarDescription::RMsg_GetAvatarDescription()
  : LmMesg(RMsg::GETAVATARDESCRIPTION, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN);
}

////
// destructor
////

RMsg_GetAvatarDescription::~RMsg_GetAvatarDescription()
{
  // empty
}

////
// Init
////

void RMsg_GetAvatarDescription::Init(lyra_id_t playerid)
{
  SetPlayerID(playerid);
}

////
// hton
////

void RMsg_GetAvatarDescription::hton()
{
  HTONL(data_.playerid);
}

////
// ntoh
////

void RMsg_GetAvatarDescription::ntoh()
{
  NTOHL(data_.playerid);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_GetAvatarDescription::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_GetAvatarDescription[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("player=%u>\n"), PlayerID());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
