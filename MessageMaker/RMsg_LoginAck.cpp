// RMsg_LoginAck.cpp  -*- C++ -*-
// $Id: RMsg_LoginAck.cpp,v 1.13 1997-11-17 14:05:06-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_LoginAck.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock2.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#include "LmSockAddrInet.h"
#endif /* WIN32 */
#include <stdio.h>
#include <string.h>

#include "RMsg_LoginAck.h"
#include "RMsg.h"
#include "LyraDefs.h"

#ifndef USE_INLINE
#include "RMsg_LoginAck.i"
#endif

////
// constructor
////

RMsg_LoginAck::RMsg_LoginAck()
  : LmMesg(RMsg::LOGINACK, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(LOGIN_UNKNOWN, Lyra::ID_UNKNOWN, Lyra::ID_UNKNOWN, INADDR_ANY, 0);
}

////
// destructor
////

RMsg_LoginAck::~RMsg_LoginAck()
{
  // empty
}

////
// Init
////

void RMsg_LoginAck::Init(int status, lyra_id_t roomid, lyra_id_t levelid)
{
  SetStatus(status);
  SetRoomID(roomid);
  SetLevelID(levelid);
}

void RMsg_LoginAck::Init(int status, lyra_id_t roomid, lyra_id_t levelid, unsigned long server_ip, int server_port)
{
  SetStatus(status);
  SetRoomID(roomid);
  SetLevelID(levelid);
  SetServerIP(server_ip);
  SetServerPort(server_port);
}

////
// hton
////

void RMsg_LoginAck::hton()
{
  HTONL(data_.status);
  HTONL(data_.roomid);
  HTONL(data_.levelid);
  HTONL(data_.server_port);
  // not converted: server_ip
}

////
// ntoh
////

void RMsg_LoginAck::ntoh()
{
  NTOHL(data_.status);
  NTOHL(data_.roomid);
  NTOHL(data_.levelid);
  NTOHL(data_.server_port);
  // not converted: server_ip
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_LoginAck::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_LoginAck[%p,%d]: "), this, sizeof(RMsg_LoginAck));
#ifndef WIN32 // relies on server classes
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("level=%u room=%u status=%c server=%s:%d>\n"),
	    LevelID(), RoomID(), Status(), LmSockAddrInet::AddressString(ServerIP()), ServerPort());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
#endif
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
