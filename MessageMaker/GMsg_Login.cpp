// GMsg_Login.cpp  -*- C++ -*-
// $Id: GMsg_Login.cpp,v 1.12 1998-04-17 16:37:44-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_Login.h"
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

#include "GMsg_Login.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_Login.i"
#endif

////
// constructor
////

GMsg_Login::GMsg_Login()
  : LmMesg(GMsg::LOGIN, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(DEFAULT_VERSION, _T("name"),  Lyra::PORT_UNKNOWN, 0, 0, 0);
}

////
// destructor
////

GMsg_Login::~GMsg_Login()
{
  // empty
}

////
// Init
////

void GMsg_Login::Init(int version, const TCHAR* playername, int serv_port, short pmare_type, int subversion,
					   short tcp_only)
{
  SetVersion(version);
  SetSubVersion(subversion);
  SetPlayerName(playername);
  SetServerPort(serv_port);
  SetPMareType(pmare_type);
  //SetFirewall(firewall);
  SetTCPOnly(tcp_only);
}

////
// hton
////

void GMsg_Login::hton()
{
  HTONL(data_.version);
  HTONL(data_.subversion);
  HTONL(data_.serv_port);
//  HTONL(data_.firewall);
  HTONS(data_.pmare_type);
  HTONS(data_.tcp_only);
  // no conversion: playername, password
}

////
// ntoh
////

void GMsg_Login::ntoh()
{
  NTOHL(data_.version);
  NTOHL(data_.subversion);
  NTOHL(data_.serv_port);
  //NTOHL(data_.firewall);
  NTOHS(data_.pmare_type);
  NTOHS(data_.tcp_only);
  // no conversion: playername, password, description
  calc_size();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_Login::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_Login[%p,%d]: "), this, sizeof(GMsg_Login));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("pmare_type=%u version=%d/%d name='%s' hash=<NOT PRINTED> servport=%d tcp_only=%d>\n"),
	    PMareType(), Version(), SubVersion(), PlayerName(), ServerPort(), TCPOnly());
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

void GMsg_Login::SetPlayerName(const TCHAR* playername)
{
 _tcsnccpy(data_.playername, playername, sizeof(data_.playername));
  TRUNC(data_.playername, sizeof(data_.playername));
}

////
// SetHash
////
/*
void GMsg_Login::SetPassword(const TCHAR* password)
{
	_tcsnccpy(data_.password, password, sizeof(data_.password));
	TRUNC(data_.password, sizeof(data_.password));
}
*/
void GMsg_Login::SetHash(const MD5Hash_t hash)
{
	memcpy((void*)data_.hash, (void*)hash, sizeof(MD5Hash_t));
}


////
// calc_size
////

void GMsg_Login::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t);
  SetMessageSize(size);
}
