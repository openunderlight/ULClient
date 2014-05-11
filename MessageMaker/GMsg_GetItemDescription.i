// GMsg_GetItemDescription.i  -*- C++ -*-
// $Id: GMsg_GetItemDescription.i,v 1.2 1997-12-11 16:25:07-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_GetItemDescription::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE const LmItemHdr& GMsg_GetItemDescription::ItemHeader() const
{
  return data_.itemheader;
}

INLINE void GMsg_GetItemDescription::SetItemHeader(const LmItemHdr& itemheader)
{
  data_.itemheader = itemheader;
}
