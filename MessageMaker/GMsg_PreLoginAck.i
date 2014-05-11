// GMsg_PreLoginAck.i  -*- C++ -*-
// $Id: GMsg_PreLoginAck.i,v 1.11 1997-10-01 17:53:07-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_PreLoginAck::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_PreLoginAck::Version() const
{
  return data_.version;
}

INLINE int GMsg_PreLoginAck::Status() const
{
  return data_.status;
}

INLINE const TCHAR* GMsg_PreLoginAck::Challenge() const
{
  return data_.challenge;
}


INLINE void GMsg_PreLoginAck::SetVersion(int version)
{
  data_.version = version;
}

INLINE void GMsg_PreLoginAck::SetStatus(int status)
{
  data_.status = status;
}







