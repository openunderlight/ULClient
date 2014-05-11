// RMsg_PlayerUpdate.cpp  -*- C++ -*-
// $Id: RMsg_PlayerUpdate.cpp,v 1.7 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_PlayerUpdate.h"
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

#include "RMsg_PlayerUpdate.h"
#include "LyraDefs.h"
#include "RMsg.h"

#ifndef USE_INLINE
#include "RMsg_PlayerUpdate.i"
#endif

////
// constructor
////

RMsg_PlayerUpdate::RMsg_PlayerUpdate()
  : LmMesg(RMsg::PLAYERUPDATE, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, DEFAULT_NUMPLAYERS);
}

////
// destructor
////

RMsg_PlayerUpdate::~RMsg_PlayerUpdate()
{
  // empty
}

////
// Init
////

void RMsg_PlayerUpdate::Init(lyra_id_t playerid, int num_players)
{
  SetPlayerID(playerid);
  SetNumPlayers(num_players);
}

////
// hton
////

void RMsg_PlayerUpdate::hton()
{
  HTONL(data_.playerid);
  for (int i = 0; i < NumPlayers(); ++i) {
    data_.players[i].ConvertToNetwork();
  }
}

////
// ntoh
////

void RMsg_PlayerUpdate::ntoh()
{
  calc_players(); // calculate num_players_, calls calc_size()
  NTOHL(data_.playerid);
  for (int i = 0; i < NumPlayers(); ++i) {
    data_.players[i].ConvertToHost();
  }
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_PlayerUpdate::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_PlayerUpdate[%p,%d]: "), this, sizeof(RMsg_PlayerUpdate));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("playerid=%u players=%d>\n"), PlayerID(), NumPlayers());
    for (int i = 0; i < NumPlayers(); ++i) {
      PeerUpdate(i).Dump(f, indent + 1);
    }
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

void RMsg_PlayerUpdate::SetNumPlayers(int num)
{
  // prevent overflow
  num_players_ = MIN(num, MAX_PLAYERS);
  calc_size();
}

////
// calc_size - calculate message size based on current number of players
////

void RMsg_PlayerUpdate::calc_size()
{
  // initial size: overall size minus variable-length field
  int size = sizeof(data_t) - sizeof(data_.players);
  // add space for players
  size += NumPlayers() * sizeof(LmPeerUpdate);
  SetMessageSize(size);
}

////
// calc_players - calculate number of players based on current message size
////

void RMsg_PlayerUpdate::calc_players()
{
  int msgsize = MessageSize();
  // determine portion of message size related to fixed fields
  int fixed_size = sizeof(data_t) - sizeof(data_.players);
  // subtract this from the overall message size; result is the
  // size attributable to the variable-sized field
  msgsize -= fixed_size;
  // calculate number of players
  SetNumPlayers(msgsize / sizeof(LmPeerUpdate));
}
