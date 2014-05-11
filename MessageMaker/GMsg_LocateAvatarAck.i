// GMsg_LocateAvatarAck.i  -*- C++ -*-
// $Id: GMsg_LocateAvatarAck.i,v 1.1 1997-11-25 15:53:47-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_LocateAvatarAck::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_LocateAvatarAck::NumPlayers() const
{
  return num_players_;
}

INLINE const TCHAR* GMsg_LocateAvatarAck::PlayerName(int player_num) const
{
  return data_.players[player_num].playername;
}

INLINE lyra_id_t GMsg_LocateAvatarAck::LevelID(int player_num) const
{
  return data_.players[player_num].levelid;
}

INLINE lyra_id_t GMsg_LocateAvatarAck::RoomID(int player_num) const
{
  return data_.players[player_num].roomid;
}

INLINE int GMsg_LocateAvatarAck::Status(int player_num) const
{
  return data_.players[player_num].status;
}

INLINE void GMsg_LocateAvatarAck::SetStatus(int player_num, int status)
{
  data_.players[player_num].status = status;
}

INLINE void GMsg_LocateAvatarAck::SetLocation(int player_num, lyra_id_t levelid, lyra_id_t roomid)
{
  data_.players[player_num].levelid = levelid;
  data_.players[player_num].roomid = roomid;
}
