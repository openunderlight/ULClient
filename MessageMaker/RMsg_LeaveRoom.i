// RMsg_LeaveRoom.i  -*- C++ -*-
// $Id: RMsg_LeaveRoom.i,v 1.5 1997-07-18 17:26:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_LeaveRoom::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t RMsg_LeaveRoom::PlayerID() const
{
  return data_.playerid;
}

INLINE int RMsg_LeaveRoom::Status() const
{
  return data_.status;
}

INLINE int RMsg_LeaveRoom::LastX() const
{
  return data_.lastx;
}

INLINE int RMsg_LeaveRoom::LastY() const
{
  return data_.lasty;
}

INLINE void RMsg_LeaveRoom::SetStatus(int status)
{
  data_.status = status;
}

INLINE void RMsg_LeaveRoom::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void RMsg_LeaveRoom::SetLastPosition(int lastx, int lasty)
{
  data_.lastx = lastx;
  data_.lasty = lasty;
}
