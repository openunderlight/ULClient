// GMsg_LocateMaresAck.i  -*- C++ -*-
// $Id: GMsg_LocateMaresAck.i,v 1.1 1997-11-25 15:53:47-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_LocateMaresAck::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_LocateMaresAck::NumPlayers() const
{
  return num_players_;
}

INLINE const TCHAR* GMsg_LocateMaresAck::PlayerName(int player_num) const
{
  return data_.players[player_num].playername;
}

INLINE short GMsg_LocateMaresAck::LevelID(int player_num) const
{
  return data_.players[player_num].levelid;
}

INLINE short GMsg_LocateMaresAck::RoomID(int player_num) const
{
  return data_.players[player_num].roomid;
}


INLINE void GMsg_LocateMaresAck::SetLocation(int player_num, short levelid, short roomid)
{
  data_.players[player_num].levelid = levelid;
  data_.players[player_num].roomid = roomid;
}

