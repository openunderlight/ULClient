// RMsg_EnterRoom.cpp  -*- C++ -*-
// $Id: RMsg_EnterRoom.cpp,v 1.11 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "RMsg_EnterRoom.h"
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

#include "RMsg_EnterRoom.h"
#include "LyraDefs.h"
#include "RMsg.h"
#include "RmRemotePlayer.h"

#ifndef USE_INLINE
#include "RMsg_EnterRoom.i"
#endif

////
// constructor
////

RMsg_EnterRoom::RMsg_EnterRoom()
  : LmMesg(RMsg::ENTERROOM, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(DEFAULT_NUMPLAYERS);
  for (int i = 0; i < MAX_PLAYERS; ++i) {
    SetPlayer(i, RmRemotePlayer::DEFAULT_INSTANCE);
  }
}

////
// destructor
////

RMsg_EnterRoom::~RMsg_EnterRoom()
{
  // empty
}

////
// Init
////

void RMsg_EnterRoom::Init(int num_players)
{
  SetNumPlayers(num_players);
}

////
// hton
////

void RMsg_EnterRoom::hton()
{
  for (int i = 0; i < NumPlayers(); ++i) {
    data_.players[i].ConvertToNetwork();
  }
}

////
// ntoh
////

void RMsg_EnterRoom::ntoh()
{
  calc_players(); // compute number of players, calls calc_size()
  for (int i = 0; i < NumPlayers(); ++i) {
    data_.players[i].ConvertToHost();
  }
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_EnterRoom::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_EnterRoom[%p,%d]: "), this, sizeof(RMsg_EnterRoom));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("players=%d>\n"), NumPlayers());
    for (int i = 0; i < NumPlayers(); ++i) {
      Player(i).Dump(f, indent + 1);
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

void RMsg_EnterRoom::SetNumPlayers(int num)
{
  // prevent overflow
  num_players_ = MIN(num, MAX_PLAYERS);
  calc_size();
}

////
// calc_size - calculate message size based on number of players
////

void RMsg_EnterRoom::calc_size()
{
  // initial size: overall size minus variable-length field
  int size = sizeof(data_t) - sizeof(data_.players);
  // add space for players
  size += NumPlayers() * sizeof(RmRemotePlayer);
  SetMessageSize(size);
}

////
// calc_players - calculate number of players based on current message size
////

void RMsg_EnterRoom::calc_players()
{
  int msgsize = MessageSize();
  // determine portion of message size related to fixed fields
  int fixed_size = sizeof(data_t) - sizeof(data_.players);
  // subtract this from the overall message size; result is the
  // size attributable to the variable-sized field
  msgsize -= fixed_size;
  // calculate number of players
  SetNumPlayers(msgsize / sizeof(RmRemotePlayer));
}
