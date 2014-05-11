// GMsg_RcvGoalText.i  -*- C++ -*-
// $Id: GMsg_RcvGoalText.i,v 1.5 1997-08-18 18:00:57-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_RcvGoalText::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_RcvGoalText::GoalID() const
{
  return data_.goalid;
}

INLINE short GMsg_RcvGoalText::SugSphere() const
{
  return data_.sugsphere;
}

INLINE short GMsg_RcvGoalText::SugStat() const
{
  return data_.sugstat;
}

INLINE const TCHAR* GMsg_RcvGoalText::Creator() const
{
  return data_.creator;
}

INLINE const TCHAR* GMsg_RcvGoalText::GoalText() const
{
  return data_.goaltext;
}

INLINE void GMsg_RcvGoalText::SetGoalID(lyra_id_t goalid)
{
  data_.goalid = goalid;
}

INLINE void GMsg_RcvGoalText::SetSugSphere(short sugsphere)
{
  data_.sugsphere = sugsphere;
}

INLINE void GMsg_RcvGoalText::SetSugStat(short sugstat)
{
  data_.sugstat = sugstat;
}

INLINE lyra_id_t GMsg_RcvGoalText::CreatorID() const
{
  return creatorid_;
}

INLINE void GMsg_RcvGoalText::SetCreatorID(lyra_id_t creatorid)
{
  creatorid_ = creatorid;
}
