// GMsg_Logout.i  -*- C++ -*-
// $Id: GMsg_Logout.i,v 1.6 1997-09-25 16:26:38-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_Logout::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE bool GMsg_Logout::item_num_ok(int item_num)
{
  return INDEX_OK(item_num, 0, Lyra::INVENTORY_MAX);
}

INLINE int GMsg_Logout::Status() const
{
  return data_.status;
}

INLINE int GMsg_Logout::NumItems() const
{
  return data_.num_items;
}

INLINE int GMsg_Logout::ItemSerial(int num) const
{
  return item_num_ok(num) ? data_.items[num].serial : 0;
}

INLINE int GMsg_Logout::ItemPosition(int num) const
{
  return item_num_ok(num) ? data_.items[num].pos & 0xFF : 0;
}

INLINE int GMsg_Logout::ItemFlags(int num) const
{
  return item_num_ok(num) ? data_.items[num].flags & 0xFF : 0;
}

INLINE int GMsg_Logout::ItemX(int itemnum) const
{
  // x_value: low 8 bits = position, high 8 bits = flags
  return (ItemFlags(itemnum) << 8) | ItemPosition(itemnum);
}

INLINE void GMsg_Logout::SetStatus(int status)
{
  data_.status = status;
}

INLINE void GMsg_Logout::SetItem(int num, int serial, int pos, int flags)
{
  if (item_num_ok(num)) {
    data_.items[num].serial = serial;
    data_.items[num].pos = pos & 0xFF;
    data_.items[num].flags = flags & 0xFF;
  }
}
