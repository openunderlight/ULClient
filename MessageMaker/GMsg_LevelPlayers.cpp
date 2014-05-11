// GMsg_LevelPlayers.cpp  -*- C++ -*-
// $Id: GMsg_LevelPlayers.cpp,v 1.5 1997-09-23 02:32:26-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_LevelPlayers.h"
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

#include "GMsg_LevelPlayers.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_LevelPlayers.i"
#endif

////
// constructor
////

GMsg_LevelPlayers::GMsg_LevelPlayers()
  : LmMesg(GMsg::LEVELPLAYERS, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, 0);
  for (int i = 0; i < Lyra::MAX_LEVELROOMS; ++i) {
    SetRoomID(i, Lyra::ID_UNKNOWN);
    SetNumPlayers(i, 0);
    SetNumAgents(i, 0);
  }
}

////
// destructor
////

GMsg_LevelPlayers::~GMsg_LevelPlayers()
{
  // empty
}

////
// Init
////

void GMsg_LevelPlayers::Init(lyra_id_t levelid, int num_rooms)
{
  SetLevelID(levelid);
  SetNumRooms(num_rooms);
}

////
// hton
////

void GMsg_LevelPlayers::hton()
{
  HTONL(data_.levelid);
  // convert room info before num_rooms
  for (int i = 0; i < NumRooms(); ++i) {
    HTONL(data_.rooms[i].roomid);
    HTONS(data_.rooms[i].num_players);
    HTONS(data_.rooms[i].num_agents);
  }
  HTONL(data_.num_rooms);
}

////
// ntoh
////

void GMsg_LevelPlayers::ntoh()
{
  NTOHL(data_.levelid);
  NTOHL(data_.num_rooms);
  for (int i = 0; i < NumRooms(); ++i) {
    NTOHL(data_.rooms[i].roomid);
    NTOHS(data_.rooms[i].num_players);
    NTOHS(data_.rooms[i].num_agents);
  }
  calc_size(); // variable-length message
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_LevelPlayers::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_LevelPlayers[%p,%d]: "), this, sizeof(GMsg_LevelPlayers));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("level=%u rooms=%d>\n"), LevelID(), NumRooms());
    for (int i = 0; i < NumRooms(); ++i) {
      INDENT(indent + 1, f);
     _ftprintf(f, _T("room %u => players=%d agents=%d\n"), RoomID(i), NumPlayers(i), NumAgents(i));
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
// PlayersInRoom
////

int GMsg_LevelPlayers::PlayersInRoom(lyra_id_t roomid) const
{
  // linear search for given room
  for (int i = 0; i < NumRooms(); ++i) {
    if (RoomID(i) == roomid) {
      return NumPlayers(i);
    }
  }
  return -1; // not found!
}

////
// AgentsInRoom
////

int GMsg_LevelPlayers::AgentsInRoom(lyra_id_t roomid) const
{
  // linear search for given room
  for (int i = 0; i < NumRooms(); ++i) {
    if (RoomID(i) == roomid) {
      return NumAgents(i);
    }
  }
  return -1; // not found!
}

////
// SetNumRooms
////

void GMsg_LevelPlayers::SetNumRooms(int num_rooms)
{
  data_.num_rooms = MIN(num_rooms, Lyra::MAX_LEVELROOMS);
  calc_size();
}

////
// calc_size
////

void GMsg_LevelPlayers::calc_size()
{
  // initial size: overall size minus variable-length field
  int size = sizeof(data_t) - sizeof(data_.rooms);
  // add space for items
  size += NumRooms() * sizeof(roominfo_t);
  SetMessageSize(size);
}
