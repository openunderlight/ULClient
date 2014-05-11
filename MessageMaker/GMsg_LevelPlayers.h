// GMsg_LevelPlayers.h  -*- C++ -*-
// $Id: GMsg_LevelPlayers.h,v 1.6 1997-09-23 02:32:26-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// message class template

#ifndef INCLUDED_GMsg_LevelPlayers
#define INCLUDED_GMsg_LevelPlayers

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_LevelPlayers : public LmMesg {

public:

  GMsg_LevelPlayers();
  ~GMsg_LevelPlayers();

  void Init(lyra_id_t levelid, int num_rooms = 0);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t LevelID() const;
  int NumRooms() const;
  lyra_id_t RoomID(int index) const;
  int NumPlayers(int index) const;
  int NumAgents(int index) const;
  int PlayersInRoom(lyra_id_t roomid) const;
  int AgentsInRoom(lyra_id_t roomid) const;

  // mutators
  void SetLevelID(lyra_id_t levelid);
  void SetNumRooms(int num_rooms);
  void SetRoomID(int index, lyra_id_t roomid);
  void SetNumPlayers(int index, int num_players);
  void SetNumAgents(int index, int num_agents);

public: // logically private

  // information on a room
  struct roominfo_t {
    lyra_id_t roomid;
    short num_players;
    short num_agents;
  };

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual size
  void calc_size();

  // message data structure
  struct data_t {
    lyra_id_t levelid;
    int num_rooms;
    roominfo_t rooms[Lyra::MAX_LEVELROOMS];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_LevelPlayers.i"
#endif

#endif /* INCLUDED_GMsg_LevelPlayers */
