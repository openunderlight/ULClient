// GMsg_SummonAvatar.cpp  -*- C++ -*-
// $Id: GMsg_SummonAvatar.cpp,v 1.4 1997-12-05 14:44:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_SummonAvatar.h"
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

#include "GMsg_SummonAvatar.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_SummonAvatar.i"
#endif

////
// constructor
////

GMsg_SummonAvatar::GMsg_SummonAvatar()
  : LmMesg(GMsg::LOCATEAVATAR, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0);
}

////
// destructor
////

GMsg_SummonAvatar::~GMsg_SummonAvatar()
{
  // empty
}

////
// Init
////

void GMsg_SummonAvatar::Init(int num_players)
{
  SetNumPlayers(num_players);
}

void GMsg_SummonAvatar::Init(const char* playername)
{
  SetNumPlayers(1);
  SetPlayerName(0, playername);
}

////
// hton
////

void GMsg_SummonAvatar::hton()
{
  // not converted: names
}

////
// ntoh
////

void GMsg_SummonAvatar::ntoh()
{
  calc_players();
  // not converted: names
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_SummonAvatar::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
  fprintf(f, "<GMsg_SummonAvatar[%p]: ", this);
  if (ByteOrder() == ByteOrder::HOST) {
    fprintf(f, "numplayers=%d>\n", NumPlayers());
    INDENT(indent + 1, f);
    fprintf(f, "names: ");
    for (int i = 0; i < NumPlayers(); ++i) {
      fprintf(f, "'%s' ", PlayerName(i));
    }
    fprintf(f, "\n");
  }
  else {
    fprintf(f, "(network order)>\n");
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetNumPlayers
////

void GMsg_SummonAvatar::SetNumPlayers(int num_players)
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

void GMsg_SummonAvatar::SetPlayerName(int player_num, const char* playername)
{
  strncpy(data_.players[player_num], playername, sizeof(pname_t));
  TRUNC(data_.players[player_num], sizeof(pname_t));
}

////
// calc_players - calculate num_players_ based on message size
////

void GMsg_SummonAvatar::calc_players()
{
  int msgsize = MessageSize();
  // determine portion of message size related to fixed fields
  int fixed_size = sizeof(data_t) - sizeof(data_.players);
  // subtract this from the overall message size; result is the
  // size attributable to the variable-sized field
  msgsize -= fixed_size;
  // calculate number of changes
  SetNumPlayers(msgsize / sizeof(pname_t));
}

////
// calc_size - calculate message size based on number of player names
////

void GMsg_SummonAvatar::calc_size()
{
  // initial size: overall size minus variable-length field
  int size = sizeof(data_t) - sizeof(data_.players);
  // add space for players
  size += NumPlayers() * sizeof(pname_t);
  SetMessageSize(size);
}
