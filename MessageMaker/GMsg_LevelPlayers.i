// GMsg_LevelPlayers.i  -*- C++ -*-
// $Id: GMsg_LevelPlayers.i,v 1.5 1997-09-23 02:32:26-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_LevelPlayers::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_LevelPlayers::LevelID() const
{
  return data_.levelid;
}

INLINE int GMsg_LevelPlayers::NumRooms() const
{
  return data_.num_rooms;
}

INLINE lyra_id_t GMsg_LevelPlayers::RoomID(int index) const
{
  return data_.rooms[index].roomid;
}

INLINE int GMsg_LevelPlayers::NumPlayers(int index) const
{
  return data_.rooms[index].num_players;
}

INLINE int GMsg_LevelPlayers::NumAgents(int index) const
{
  return data_.rooms[index].num_agents;
}

INLINE void GMsg_LevelPlayers::SetLevelID(lyra_id_t levelid)
{
  data_.levelid = levelid;
}

INLINE void GMsg_LevelPlayers::SetRoomID(int index, lyra_id_t roomid)
{
  data_.rooms[index].roomid = roomid;
}

INLINE void GMsg_LevelPlayers::SetNumPlayers(int index, int num_players)
{
  data_.rooms[index].num_players = num_players;
}

INLINE void GMsg_LevelPlayers::SetNumAgents(int index, int num_agents)
{
  data_.rooms[index].num_agents = num_agents;
}
