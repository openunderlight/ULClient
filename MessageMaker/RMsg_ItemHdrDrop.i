// RMsg_ItemHdrDrop.i  -*- C++ -*-
// $Id: RMsg_ItemHdrDrop.i,v 1.6 1997-07-18 17:26:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_ItemHdrDrop::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int RMsg_ItemHdrDrop::NumItems() const
{
  return num_items_;
}

INLINE const LmItemHdr& RMsg_ItemHdrDrop::ItemHeader(int num) const
{
  return data_.itemhdrs[num].itemhdr;
}

INLINE const LmPosition& RMsg_ItemHdrDrop::Position(int num) const
{
  return data_.itemhdrs[num].pos;
}

INLINE void RMsg_ItemHdrDrop::SetItemHeader(int num, const LmItemHdr& itemhdr,
					    const LmPosition& pos)
{
  data_.itemhdrs[num].itemhdr = itemhdr;
  data_.itemhdrs[num].pos = pos;
}

INLINE void RMsg_ItemHdrDrop::SetItem(int num, const LmItem& item, const LmPosition& pos)
{
  data_.itemhdrs[num].itemhdr = item.Header();
  data_.itemhdrs[num].pos = pos;
}
