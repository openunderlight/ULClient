// RMsg_GotoRoom.i  -*- C++ -*-
// $Id: RMsg_GotoRoom.i,v 1.6 1997-07-18 17:26:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_GotoRoom::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t RMsg_GotoRoom::RoomID() const
{
  return data_.roomid;
}

INLINE const LmPeerUpdate& RMsg_GotoRoom::PeerUpdate() const
{
  return data_.update;
}

INLINE int RMsg_GotoRoom::LastX() const
{
  return data_.lastx;
}

INLINE int RMsg_GotoRoom::LastY() const
{
  return data_.lasty;
}

INLINE void RMsg_GotoRoom::SetRoomID(lyra_id_t roomid)
{
  data_.roomid = roomid;
}

INLINE void RMsg_GotoRoom::SetPeerUpdate(const LmPeerUpdate& update)
{
  data_.update = update;
}

INLINE void RMsg_GotoRoom::SetLastPosition(int lastx, int lasty)
{
  data_.lastx = lastx;
  data_.lasty = lasty;
}
