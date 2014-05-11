// RMsg_JoinedParty.i  -*- C++ -*-
// $Id: RMsg_JoinedParty.i,v 1.4 1997-07-18 17:26:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_JoinedParty::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE const RmRemotePlayer& RMsg_JoinedParty::PartyMember() const
{
  return data_.member;
}

INLINE void RMsg_JoinedParty::SetPartyMember(const RmRemotePlayer& member)
{
  data_.member = member;
}
