// RMsg_RoomLoginAck.i  -*- C++ -*-
// $Id: RMsg_RoomLoginAck.i,v 1.9 1997-11-17 14:05:06-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_RoomLoginAck::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE short RMsg_RoomLoginAck::Status() const
{
  return data_.status;
}

INLINE short RMsg_RoomLoginAck::NumNeighbors() const
{
  return data_.num_neighbors;
}


INLINE void RMsg_RoomLoginAck::SetStatus(short status)
{
  data_.status = status;
}

INLINE void RMsg_RoomLoginAck::SetNumNeighbors(short num_neighbors)
{
  data_.num_neighbors = num_neighbors;
}
