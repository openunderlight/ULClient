// RmRemotePlayer.i  -*- C++ -*-
// $Id: RmRemotePlayer.i,v 1.5 1997-07-28 21:59:12-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void RmRemotePlayer::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t RmRemotePlayer::PlayerID() const
{
  return rp_.playerid;
}

INLINE lyra_id_t RmRemotePlayer::Room() const
{
  return rp_.room;
}

INLINE const LmAvatar& RmRemotePlayer::Avatar() const
{
  return rp_.avatar;
}

INLINE const TCHAR* RmRemotePlayer::PlayerName() const
{
  return rp_.playername;
}

INLINE const LmPeerUpdate& RmRemotePlayer::PeerUpdate() const
{
  return rp_.update;
}


INLINE void RmRemotePlayer::SetAvatar(const LmAvatar& avatar)
{
  rp_.avatar = avatar;
}

INLINE void RmRemotePlayer::SetRoom(lyra_id_t room)
{
  rp_.room = room;
}

INLINE void RmRemotePlayer::SetPlayerID(lyra_id_t playerid)
{
  rp_.playerid = playerid;
}



INLINE void RmRemotePlayer::SetPeerUpdate(const LmPeerUpdate& update)
{
  rp_.update = update;
}



