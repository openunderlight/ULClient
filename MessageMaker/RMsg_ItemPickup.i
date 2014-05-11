// RMsg_ItemPickup.i  -*- C++ -*-
// $Id: RMsg_ItemPickup.i,v 1.8 1997-07-18 17:26:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_ItemPickup::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int RMsg_ItemPickup::NumItems() const
{
  return num_items_;
}

INLINE const LmItemHdr& RMsg_ItemPickup::ItemHeader(int num) const
{
  return data_.itemhdrs[num];
}

INLINE void RMsg_ItemPickup::SetItemHeader(int num, const LmItemHdr& itemhdr)
{
  data_.itemhdrs[num] = itemhdr;
}

INLINE void RMsg_ItemPickup::SetItem(int num, const LmItem& item)
{
  data_.itemhdrs[num] = item.Header();
}
