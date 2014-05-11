// GMsg_Goal.i  -*- C++ -*-
// $Id: GMsg_Goal.i,v 1.3 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_Goal::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_Goal::ID() const
{
  return data_.id;
}

INLINE int GMsg_Goal::RequestType() const
{
  return data_.messagetyp;
}

INLINE void GMsg_Goal::SetID(lyra_id_t id)
{
  data_.id = id;
}

INLINE void GMsg_Goal::SetRequestType(int messagetyp)
{
  data_.messagetyp = messagetyp;
}
