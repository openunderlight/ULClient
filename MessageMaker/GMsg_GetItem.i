// GMsg_GetItem.i  -*- C++ -*-
// $Id: GMsg_GetItem.i,v 1.4 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_GetItem::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_GetItem::RoomID() const
{
  return data_.roomid;
}

INLINE const LmItemHdr& GMsg_GetItem::ItemHeader() const
{
  return data_.itemhdr;
}

INLINE void GMsg_GetItem::SetRoomID(lyra_id_t roomid)
{
  data_.roomid = roomid;
}

INLINE void GMsg_GetItem::SetItemHeader(const LmItemHdr& itemhdr)
{
  data_.itemhdr = itemhdr;
}
