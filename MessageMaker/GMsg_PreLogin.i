// GMsg_PreLogin.i  -*- C++ -*-
// $Id: GMsg_PreLogin.i,v 1.8 1998-04-17 16:37:44-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_PreLogin::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_PreLogin::Version() const
{
  return data_.version;
}


INLINE void GMsg_PreLogin::SetVersion(int version)
{
  data_.version = version;
}

