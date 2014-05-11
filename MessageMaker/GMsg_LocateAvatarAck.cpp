// GMsg_LocateAvatarAck.cpp  -*- C++ -*-
// $Id: GMsg_LocateAvatarAck.cpp,v 1.1 1997-11-25 15:53:47-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_LocateAvatarAck.h"
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

#include "GMsg_LocateAvatarAck.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_LocateAvatarAck.i"
#endif

////
// constructor
////

GMsg_LocateAvatarAck::GMsg_LocateAvatarAck()
  : LmMesg(GMsg::LOCATEAVATARACK, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0);
}

////
// destructor
////

GMsg_LocateAvatarAck::~GMsg_LocateAvatarAck()
{
  // empty
}

////
// Init
////

void GMsg_LocateAvatarAck::Init(int num_players)
{
  SetNumPlayers(num_players);
}

////
// hton
////

void GMsg_LocateAvatarAck::hton()
{
  for (int i = 0; i < NumPlayers(); ++i) {
    // not converted: PlayerName
    HTONL(data_.players[i].levelid);
    HTONL(data_.players[i].roomid);
    HTONL(data_.players[i].status);
  }
}

////
// ntoh
////

void GMsg_LocateAvatarAck::ntoh()
{
  calc_players();
  for (int i = 0; i < NumPlayers(); ++i) {
    // not converted: PlayerName
    NTOHL(data_.players[i].levelid);
    NTOHL(data_.players[i].roomid);
    NTOHL(data_.players[i].status);
  }
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_LocateAvatarAck::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_LocateAvatarAck[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("numplayers=%d>\n"), NumPlayers());
    for (int i = 0; i < NumPlayers(); ++i) {
      INDENT(indent + 1, f);
     _ftprintf(f, _T("player='%s' status=%c level=%u room=%u\n"), PlayerName(i), Status(i), LevelID(i), RoomID(i));
    }
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetNumPlayers
////

void GMsg_LocateAvatarAck::SetNumPlayers(int num_players)
{
  // avoid bad range
  if ((num_players < 0) || (num_players > MAX_PLAYERS)) {
    num_players_ = 0;
  }
  else {
    num_players_ = num_players;
  }
  calc_size();
}

////
// SetPlayerName
////

void GMsg_LocateAvatarAck::SetPlayerName(int player_num, const TCHAR* playername)
{
 _tcsnccpy(data_.players[player_num].playername, playername, sizeof(data_.players[player_num].playername));
  TRUNC(data_.players[player_num].playername, sizeof(data_.players[player_num].playername));
}

////
// calc_players - calculate num_players_ based on message size
////

void GMsg_LocateAvatarAck::calc_players()
{
  int msgsize = MessageSize();
  // determine portion of message size related to fixed fields
  int fixed_size = sizeof(data_t) - sizeof(data_.players);
  // subtract this from the overall message size; result is the
  // size attributable to the variable-sized field
  msgsize -= fixed_size;
  // calculate number of players
  SetNumPlayers(msgsize / sizeof(pinfo_t));
}

////
// calc_size - calculate message size based on number of player names
////

void GMsg_LocateAvatarAck::calc_size()
{
  // initial size: overall size minus variable-length field
  int size = sizeof(data_t) - sizeof(data_.players);
  // add space for players
  size += NumPlayers() * sizeof(pinfo_t);
  SetMessageSize(size);
}
