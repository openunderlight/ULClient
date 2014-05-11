// GMsg_TakeItemAck.i  -*- C++ -*-
// $Id: GMsg_TakeItemAck.i,v 1.1 1997-09-18 16:51:12-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_TakeItemAck::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_TakeItemAck::Status() const
{
  return data_.status;
}

INLINE const LmItemHdr& GMsg_TakeItemAck::ItemHeader() const
{
  return data_.itemheader;
}

INLINE void GMsg_TakeItemAck::SetStatus(int status)
{
  data_.status = status;
}

INLINE void GMsg_TakeItemAck::SetItemHeader(const LmItemHdr& itemheader)
{
  data_.itemheader = itemheader;
}
