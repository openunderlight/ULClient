// RMsg_ItemDrop.i  -*- C++ -*-
// $Id: RMsg_ItemDrop.i,v 1.8 1997-07-18 17:26:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_ItemDrop::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int RMsg_ItemDrop::NumItems() const
{
  return num_items_;
}

INLINE const LmItem& RMsg_ItemDrop::Item(int num) const
{
  return data_.items[num].item;
}

INLINE const LmPosition& RMsg_ItemDrop::Position(int num) const
{
  return data_.items[num].pos;
}

INLINE void RMsg_ItemDrop::SetItem(int num, const LmItem& item, const LmPosition& pos)
{
  data_.items[num].item = item;
  data_.items[num].pos = pos;
}
