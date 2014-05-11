// GMsg_GotoLevel.i  -*- C++ -*-
// $Id: GMsg_GotoLevel.i,v 1.4 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_GotoLevel::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_GotoLevel::LevelID() const
{
  return data_.levelid;
}

INLINE lyra_id_t GMsg_GotoLevel::RoomID() const
{
  return data_.roomid;
}

INLINE const LmPeerUpdate& GMsg_GotoLevel::PeerUpdate() const
{
  return data_.update;
}

INLINE void GMsg_GotoLevel::SetLevelID(lyra_id_t levelid)
{
  data_.levelid = levelid;
}

INLINE void GMsg_GotoLevel::SetRoomID(lyra_id_t roomid)
{
  data_.roomid = roomid;
}

INLINE void GMsg_GotoLevel::SetPeerUpdate(const LmPeerUpdate& update)
{
  data_.update = update;
}
