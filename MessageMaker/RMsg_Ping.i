// RMsg_Ping.i  -*- C++ -*-
// $Id: RMsg_Ping.i,v 1.3 1997-07-18 17:26:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_Ping::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int RMsg_Ping::Nonce() const
{
  return data_.nonce;
}

INLINE void RMsg_Ping::SetNonce(int nonce)
{
  data_.nonce = nonce;
}
