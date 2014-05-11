// GMsg_TakeItem.i  -*- C++ -*-
// $Id: GMsg_TakeItem.i,v 1.1 1997-09-18 16:51:12-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_TakeItem::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_TakeItem::SourceID() const
{
  return data_.sourceid;
}

INLINE const LmItem& GMsg_TakeItem::Item() const
{
  return data_.item;
}

INLINE void GMsg_TakeItem::SetSourceID(lyra_id_t sourceid)
{
  data_.sourceid = sourceid;
}

INLINE void GMsg_TakeItem::SetItem(const LmItem& item)
{
  data_.item = item;
}
