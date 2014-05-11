// GMsg_PostReport.i  -*- C++ -*-
// $Id: GMsg_PostReport.i,v 1.4 1997-08-06 00:41:49-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_PostReport::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_PostReport::GoalID() const
{
  return data_.goalid;
}

INLINE int GMsg_PostReport::AwardXP() const
{
  return data_.awardxp;
}

INLINE const TCHAR* GMsg_PostReport::Recipient() const
{
  return data_.recipient;
}

INLINE const TCHAR* GMsg_PostReport::Summary() const
{
  return data_.summary;
}

INLINE const TCHAR* GMsg_PostReport::ReportText() const
{
  return data_.reporttext;
}

INLINE void GMsg_PostReport::SetGoalID(lyra_id_t goalid)
{
  data_.goalid = goalid;
}

INLINE void GMsg_PostReport::SetAwardXP(int awardxp)
{
  data_.awardxp = awardxp;
}
