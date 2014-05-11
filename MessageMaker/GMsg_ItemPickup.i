// GMsg_ItemPickup.i  -*- C++ -*-
// $Id: GMsg_ItemPickup.i,v 1.3 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_ItemPickup::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_ItemPickup::Status() const
{
  return data_.status;
}

INLINE const LmItem& GMsg_ItemPickup::Item() const
{
  return data_.item;
}

INLINE void GMsg_ItemPickup::SetStatus(int status)
{
  data_.status = status;
}

INLINE void GMsg_ItemPickup::SetItem(const LmItem& item)
{
  data_.item = item;
}
