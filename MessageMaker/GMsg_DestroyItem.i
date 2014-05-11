// GMsg_DestroyItem.i  -*- C++ -*-
// $Id: GMsg_DestroyItem.i,v 1.3 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_DestroyItem::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE const LmItemHdr& GMsg_DestroyItem::ItemHeader() const
{
  return data_.itemhdr;
}

INLINE void GMsg_DestroyItem::SetItemHeader(const LmItemHdr& itemhdr)
{
  data_.itemhdr = itemhdr;
}
