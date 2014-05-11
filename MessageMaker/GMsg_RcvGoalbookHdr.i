// GMsg_RcvGoalbookHdr.i  -*- C++ -*-
// $Id: GMsg_RcvGoalbookHdr.i,v 1.1 1997-08-18 22:00:58-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_RcvGoalbookHdr::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_RcvGoalbookHdr::GoalID() const
{
  return data_.goalid;
}

INLINE short GMsg_RcvGoalbookHdr::Guild() const
{
  return data_.guild;
}

INLINE short GMsg_RcvGoalbookHdr::Rank() const
{
  return data_.rank;
}

INLINE const TCHAR* GMsg_RcvGoalbookHdr::Summary() const
{
  return data_.summary;
}

INLINE void GMsg_RcvGoalbookHdr::SetGoalID(lyra_id_t goalid)
{
  data_.goalid = goalid;
}

INLINE void GMsg_RcvGoalbookHdr::SetGuild(short guild)
{
  data_.guild = guild;
}

INLINE void GMsg_RcvGoalbookHdr::SetRank(short rank)
{
  data_.rank = rank;
}
