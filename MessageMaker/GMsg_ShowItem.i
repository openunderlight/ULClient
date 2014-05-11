// GMsg_ShowItem.i  -*- C++ -*-
// $Id: GMsg_ShowItem.i,v 1.2 1997-12-11 16:25:07-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_ShowItem::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_ShowItem::TargetID() const
{
  return data_.targetid;
}

INLINE const LmItemHdr& GMsg_ShowItem::ItemHeader() const
{
  return data_.itemheader;
}

INLINE void GMsg_ShowItem::SetTargetID(lyra_id_t targetid)
{
  data_.targetid = targetid;
}

INLINE void GMsg_ShowItem::SetItemHeader(const LmItemHdr& itemheader)
{
  data_.itemheader = itemheader;
}
