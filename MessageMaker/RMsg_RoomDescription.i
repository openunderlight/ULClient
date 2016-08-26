// RMsg_RoomDescription.i  -*- C++ -*-
// $Id: RMsg_RoomDescription.i,v 1.1 1998-04-17 16:27:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_RoomDescription::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE short RMsg_RoomDescription::LevelID() const
{
  return data_.levelid;
}

INLINE short RMsg_RoomDescription::RoomID() const
{
  return data_.roomid;
}
INLINE const TCHAR* RMsg_RoomDescription::Description() const
{
  return data_.description;
}

INLINE void RMsg_RoomDescription::SetLevelID(short levelid)
{
  data_.levelid = levelid;
}

INLINE void RMsg_RoomDescription::SetRoomID(short roomid)
{
  data_.roomid = roomid;
}