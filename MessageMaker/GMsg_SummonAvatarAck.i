// GMsg_SummonAvatarAck.i  -*- C++ -*-
// $Id: GMsg_SummonAvatarAck.i,v 1.1 1997-11-25 15:53:47-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_SummonAvatarAck::Dump(FILE*, int) const
{
	// empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_SummonAvatarAck::NumPlayers() const
{
	return num_players_;
}

INLINE const char* GMsg_SummonAvatarAck::PlayerName(int player_num) const
{
	return data_.players[player_num].playername;
}

INLINE lyra_id_t GMsg_SummonAvatarAck::LevelID(int player_num) const
{
	return data_.players[player_num].levelid;
}

INLINE lyra_id_t GMsg_SummonAvatarAck::RoomID(int player_num) const
{
	return data_.players[player_num].roomid;
}

INLINE int GMsg_SummonAvatarAck::Status(int player_num) const
{
	return data_.players[player_num].status;
}

INLINE void GMsg_SummonAvatarAck::SetStatus(int player_num, int status)
{
	data_.players[player_num].status = status;
}

INLINE void GMsg_SummonAvatarAck::SetLocation(int player_num, lyra_id_t caster_id, int x, int y, int height, int angle, lyra_id_t levelid, lyra_id_t roomid)
{
	data_.players[player_num].x = x;
	data_.players[player_num].y = y;
	data_.players[player_num].height = height;
	data_.players[player_num].angle = angle;
	data_.players[player_num].levelid = levelid;
	data_.players[player_num].roomid = roomid;
}
