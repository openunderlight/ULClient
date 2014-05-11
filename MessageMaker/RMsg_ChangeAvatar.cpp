// RMsg_ChangeAvatar.cpp  -*- C++ -*-
// $Id: RMsg_ChangeAvatar.cpp,v 1.4 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_ChangeAvatar.h"
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

#include "RMsg_ChangeAvatar.h"

#ifndef USE_INLINE
#include "RMsg_ChangeAvatar.i"
#endif

////
// constructor
////

RMsg_ChangeAvatar::RMsg_ChangeAvatar()
  : LmMesg(RMsg::CHANGEAVATAR, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, LmAvatar::DEFAULT_INSTANCE);
}

////
// destructor
////

RMsg_ChangeAvatar::~RMsg_ChangeAvatar()
{
  // empty
}

////
// Init
////

void RMsg_ChangeAvatar::Init(lyra_id_t playerid, const LmAvatar& avatar)
{
  SetPlayerID(playerid);
  SetAvatar(avatar);
}

////
// hton
////

void RMsg_ChangeAvatar::hton()
{
  HTONL(data_.playerid);
  data_.avatar.ConvertToNetwork();
}

////
// ntoh
////

void RMsg_ChangeAvatar::ntoh()
{
  NTOHL(data_.playerid);
  data_.avatar.ConvertToHost();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_ChangeAvatar::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_ChangeAvatar[%p,%d]: "), this, sizeof(RMsg_ChangeAvatar));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("player=%u>\n"), PlayerID());
    Avatar().Dump(f, indent + 1);
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
