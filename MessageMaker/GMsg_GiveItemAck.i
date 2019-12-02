// GMsg_GiveItemAck.i  -*- C++ -*-
// $Id: GMsg_GiveItemAck.i,v 1.1 1997-09-19 16:42:16-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_GiveItemAck::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_GiveItemAck::Status() const
{
  return data_.status;
}

INLINE const LmItemHdr& GMsg_GiveItemAck::ItemHeader() const
{
  return data_.itemheader;
}

INLINE void GMsg_GiveItemAck::SetStatus(int status)
{
  data_.status = status;
}

INLINE void GMsg_GiveItemAck::SetItemHeader(const LmItemHdr& itemheader)
{
  data_.itemheader = itemheader;
}

INLINE void GMsg_GiveItemAck::SetTarget(lyra_id_t target) 
{
	data_.target = target;
}

INLINE lyra_id_t GMsg_GiveItemAck::Target() const
{
	return data_.target;
}
