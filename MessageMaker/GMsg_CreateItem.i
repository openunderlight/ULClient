// GMsg_CreateItem.i  -*- C++ -*-
// $Id: GMsg_CreateItem.i,v 1.4 1997-12-05 14:44:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_CreateItem::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE const LmItem& GMsg_CreateItem::Item() const
{
  return data_.item;
}

INLINE const TCHAR* GMsg_CreateItem::Description() const
{
  return data_.desc;
}

INLINE void GMsg_CreateItem::SetItem(const LmItem& item)
{
  data_.item = item;
}
