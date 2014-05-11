// GMsg_Ping.i  -*- C++ -*-
// $Id: GMsg_Ping.i,v 1.4 1997-10-22 19:21:08-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_Ping::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_Ping::Nonce() const
{
  return data_.nonce;
}

INLINE int GMsg_Ping::PingType() const
{
  return data_.ping_type;
}

INLINE void GMsg_Ping::SetNonce(int nonce)
{
  data_.nonce = nonce;
}

INLINE void GMsg_Ping::SetPingType(int ping_type)
{
  data_.ping_type = ping_type;
}
