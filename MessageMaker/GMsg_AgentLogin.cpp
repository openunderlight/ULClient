// GMsg_AgentLogin.cpp  -*- C++ -*-
// $Id: GMsg_AgentLogin.cpp,v 1.1 1997-10-15 16:22:40-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_AgentLogin.h"
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

#include "GMsg_AgentLogin.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_AgentLogin.i"
#endif

////
// constructor
////

GMsg_AgentLogin::GMsg_AgentLogin()
  : LmMesg(GMsg::AGENTLOGIN, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(DEFAULT_VERSION, _T("name"), Lyra::PORT_UNKNOWN, 0);
}

////
// destructor
////

GMsg_AgentLogin::~GMsg_AgentLogin()
{
  // empty
}

////
// Init
////

void GMsg_AgentLogin::Init(int version, const TCHAR* playername, int serv_port, lyra_id_t billing_id)
{
  SetVersion(version);
  SetPlayerName(playername);
  SetServerPort(serv_port);
  SetBillingID(billing_id);
}

////
// hton
////

void GMsg_AgentLogin::hton()
{
  HTONL(data_.version);
  HTONL(data_.serv_port);
  HTONL(data_.billing_id);
  // no conversion: playername, hash
}

////
// ntoh
////

void GMsg_AgentLogin::ntoh()
{
  NTOHL(data_.version);
  NTOHL(data_.serv_port);
  NTOHL(data_.billing_id);
  // no conversion: playername, hash
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_AgentLogin::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_AgentLogin[%p,%d]: "), this, sizeof(GMsg_AgentLogin));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("billing_id=%u version=%d name='%s' hash=<NOT PRINTED> servport=%d>\n"),
	    BillingID(), Version(), PlayerName(), ServerPort());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetPlayerName
////

void GMsg_AgentLogin::SetPlayerName(const TCHAR* playername)
{
 _tcsnccpy(data_.playername, playername, sizeof(data_.playername));
  TRUNC(data_.playername, sizeof(data_.playername));
}

/*
void GMsg_AgentLogin::SetPassword(const TCHAR* password)
{
 _tcsnccpy(data_.password, password, sizeof(data_.password));
  TRUNC(data_.password, sizeof(data_.password));
}
*/
////
// SetHash
////

void GMsg_AgentLogin::SetHash(const MD5Hash_t hash)
{
	memcpy((void*)data_.hash, (void*)hash, sizeof(MD5Hash_t));
}
