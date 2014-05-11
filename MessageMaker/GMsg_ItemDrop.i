// GMsg_ItemDrop.i  -*- C++ -*-
// $Id: GMsg_ItemDrop.i,v 1.4 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_ItemDrop::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_ItemDrop::Status() const
{
  return data_.status;
}

INLINE const LmItemHdr& GMsg_ItemDrop::ItemHeader() const
{
  return data_.itemhdr;
}

INLINE void GMsg_ItemDrop::SetStatus(int status)
{
  data_.status = status;
}

INLINE void GMsg_ItemDrop::SetItemHeader(const LmItemHdr& itemhdr)
{
  data_.itemhdr = itemhdr;
}
