// RMsg_PartyInfo.i  -*- C++ -*-
// $Id: RMsg_PartyInfo.i,v 1.5 1997-07-18 17:26:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_PartyInfo::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t RMsg_PartyInfo::LeaderID() const
{
  return data_.leaderid;
}

INLINE int RMsg_PartyInfo::PartySize() const
{
  return num_members_;
}

INLINE const RmRemotePlayer& RMsg_PartyInfo::PartyMember(int num) const
{
  return data_.members[num];
}

INLINE void RMsg_PartyInfo::SetLeaderID(lyra_id_t leaderid)
{
  data_.leaderid = leaderid;
}

INLINE void RMsg_PartyInfo::SetPartyMember(int num, const RmRemotePlayer& p)
{
  data_.members[num] = p;
}
