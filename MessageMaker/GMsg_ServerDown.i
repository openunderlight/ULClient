// GMsg_ServerDown.i  -*- C++ -*-
// $Id: GMsg_ServerDown.i,v 1.3 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_ServerDown::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_ServerDown::Status() const
{
  return data_.status;
}

INLINE void GMsg_ServerDown::SetStatus(int status)
{
  data_.status = status;
}
