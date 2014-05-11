// GMsg_DestroyRoomItem.i  -*- C++ -*-
// $Id: GMsg_DestroyRoomItem.i,v 1.3 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_DestroyRoomItem::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_DestroyRoomItem::RoomID() const
{
  return data_.roomid;
}

INLINE const LmItemHdr& GMsg_DestroyRoomItem::ItemHeader() const
{
  return data_.itemhdr;
}

INLINE void GMsg_DestroyRoomItem::SetRoomID(lyra_id_t roomid)
{
  data_.roomid = roomid;
}

INLINE void GMsg_DestroyRoomItem::SetItemHeader(const LmItemHdr& itemhdr)
{
  data_.itemhdr = itemhdr;
}
