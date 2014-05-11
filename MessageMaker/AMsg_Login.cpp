// AMsg_Login.cpp  -*- C++ -*-
// $Id: AMsg_Login.cpp,v 1.1 1997-06-17 15:34:01-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "AMsg_Login.h"
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

#include "AMsg_Login.h"
#include "LyraDefs.h"
#include "AMsg.h"

#ifndef USE_INLINE
#include "AMsg_Login.i"
#endif

////
// constructor
////

AMsg_Login::AMsg_Login()
  : LmMesg(AMsg::LOGIN, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0, _T("username"), _T("password"));
}

////
// destructor
////

AMsg_Login::~AMsg_Login()
{
  // empty
}

////
// Init
////

void AMsg_Login::Init(int version, const TCHAR* playername, const TCHAR* password)
{
  SetVersion(version);
  SetPlayerName(playername);
  SetPassword(password);
}

////
// hton
////

void AMsg_Login::hton()
{
  HTONL(data_.version);
  // no conversion: name, password
}

////
// ntoh
////

void AMsg_Login::ntoh()
{
  NTOHL(data_.version);
  // no conversion: name, password
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void AMsg_Login::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
  fprintf(f, "<AMsg_Login[%p]: ", this);
  if (ByteOrder() == ByteOrder::HOST) {
    fprintf(f, "version=%d user='%s' password='%s'>\n", Version(), PlayerName(), Password());
  }
  else {
    fprintf(f, "(network order)>\n");
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetPlayerName
////

void AMsg_Login::SetPlayerName(const TCHAR* playername)
{
  _tcsncpy(data_.playername, playername, sizeof(data_.playername));
  TRUNC(data_.playername, PLAYERNAME_MAX);
}

////
// SetPassword
////

void AMsg_Login::SetPassword(const TCHAR* password)
{
  _tcsncpy(data_.password, password, sizeof(data_.password));
  TRUNC(data_.password, PASSWORD_MAX);
}
