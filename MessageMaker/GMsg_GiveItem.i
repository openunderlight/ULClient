// GMsg_GiveItem.i  -*- C++ -*-
// $Id: GMsg_GiveItem.i,v 1.1 1997-09-18 16:51:12-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_GiveItem::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_GiveItem::TargetID() const
{
  return data_.targetid;
}

INLINE const LmItemHdr& GMsg_GiveItem::ItemHeader() const
{
  return data_.itemheader;
}

INLINE void GMsg_GiveItem::SetTargetID(lyra_id_t targetid)
{
  data_.targetid = targetid;
}

INLINE void GMsg_GiveItem::SetItemHeader(const LmItemHdr& itemheader)
{
  data_.itemheader = itemheader;
}
