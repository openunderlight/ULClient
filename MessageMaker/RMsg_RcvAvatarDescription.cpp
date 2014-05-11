// RMsg_RcvAvatarDescription.cpp  -*- C++ -*-
// $Id: RMsg_RcvAvatarDescription.cpp,v 1.1 1998-04-17 16:27:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_RcvAvatarDescription.h"
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

#include "RMsg_RcvAvatarDescription.h"
#include "LyraDefs.h"
#include "RMsg.h"

#ifndef USE_INLINE
#include "RMsg_RcvAvatarDescription.i"
#endif

////
// constructor
////

RMsg_RcvAvatarDescription::RMsg_RcvAvatarDescription()
  : LmMesg(RMsg::RCVAVATARDESCRIPTION, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, _T("none"));
}

////
// destructor
////

RMsg_RcvAvatarDescription::~RMsg_RcvAvatarDescription()
{
  // empty
}

////
// Init
////

void RMsg_RcvAvatarDescription::Init(lyra_id_t playerid, const TCHAR* description)
{
  SetPlayerID(playerid);
  SetDescription(description);
}

////
// hton
////

void RMsg_RcvAvatarDescription::hton()
{
  HTONL(data_.playerid);
  // not converted: Description
}

////
// ntoh
////

void RMsg_RcvAvatarDescription::ntoh()
{
  NTOHL(data_.playerid);
  // not converted: Description
  calc_size();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_RcvAvatarDescription::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_RcvAvatarDescription[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("player=%u desc='%s'>\n"), PlayerID(), Description());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetDescription
////

void RMsg_RcvAvatarDescription::SetDescription(const TCHAR* description)
{
 _tcsnccpy(data_.description, description, sizeof(data_.description));
  TRUNC(data_.description, sizeof(data_.description));
  calc_size();
}

////
// calc_size
////

void RMsg_RcvAvatarDescription::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.description);
  // add string length, plus 1 for null
  size += (_tcslen(data_.description) + 1);
  SetMessageSize(size);
}
