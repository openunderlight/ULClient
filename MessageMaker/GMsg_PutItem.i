// GMsg_PutItem.i  -*- C++ -*-
// $Id: GMsg_PutItem.i,v 1.5 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_PutItem::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_PutItem::RoomID() const
{
  return data_.roomid;
}

INLINE int GMsg_PutItem::TTL() const
{
  return data_.ttl;
}

INLINE const LmItemHdr& GMsg_PutItem::ItemHeader() const
{
  return data_.itemhdr;
}

INLINE const LmPosition& GMsg_PutItem::Position() const
{
  return data_.position;
}

INLINE void GMsg_PutItem::SetRoomID(lyra_id_t roomid)
{
  data_.roomid = roomid;
}

INLINE void GMsg_PutItem::SetTTL(int ttl)
{
  data_.ttl = ttl;
}

INLINE void GMsg_PutItem::SetItemHeader(const LmItemHdr& itemhdr)
{
  data_.itemhdr = itemhdr;
}

INLINE void GMsg_PutItem::SetPosition(const LmPosition& position)
{
  data_.position = position;
}
