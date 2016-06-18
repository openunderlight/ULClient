// RMsg_GetRoomDescription.i  -*- C++ -*-
// $Id: RMsg_GetRoomDescription.i,v 1.1 1998-04-17 16:27:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_GetRoomDescription::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE short RMsg_GetRoomDescription::LevelID() const
{
  return data_.levelid;
}

INLINE void RMsg_GetRoomDescription::SetLevelID(short level)
{
  data_.levelid = level;
}

INLINE short RMsg_GetRoomDescription::RoomID() const
{
  return data_.roomid;
}

INLINE void RMsg_GetRoomDescription::SetRoomID(short room)
{
  data_.roomid = room;
}