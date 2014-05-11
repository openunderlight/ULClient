// GMsg_ViewItem.i  -*- C++ -*-
// $Id: GMsg_ViewItem.i,v 1.1 1997-12-05 14:44:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_ViewItem::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_ViewItem::SourceID() const
{
  return data_.sourceid;
}

INLINE const LmItemHdr& GMsg_ViewItem::ItemHeader() const
{
  return data_.itemheader;
}

INLINE const TCHAR* GMsg_ViewItem::ItemName() const
{
  return data_.itemname;
}

INLINE void GMsg_ViewItem::SetSourceID(lyra_id_t sourceid)
{
  data_.sourceid = sourceid;
}

INLINE void GMsg_ViewItem::SetItemHeader(const LmItemHdr& itemheader)
{
  data_.itemheader = itemheader;
}
