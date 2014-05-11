// GMsg_GaspLogin.i  -*- C++ -*-
// $Id: GMsg_GaspLogin.i,v 1.1 1998-01-22 15:08:27-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_GaspLogin::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_GaspLogin::Version() const
{
  return data_.version;
}

INLINE int GMsg_GaspLogin::SubVersion() const
{
  return data_.subversion;
}

INLINE mpath_id_t GMsg_GaspLogin::MPathID() const
{
  return data_.mpathid;
}

INLINE void GMsg_GaspLogin::SetVersion(int version)
{
  data_.version = version;
}

INLINE void GMsg_GaspLogin::SetSubVersion(int subversion)
{
  data_.subversion = subversion;
}

INLINE void GMsg_GaspLogin::SetMPathID(mpath_id_t mpathid)
{
  data_.mpathid = mpathid;
}
