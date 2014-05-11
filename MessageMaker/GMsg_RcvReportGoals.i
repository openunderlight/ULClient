// GMsg_RcvReportGoals.i  -*- C++ -*-
// $Id: GMsg_RcvReportGoals.i,v 1.5 1998-02-03 23:33:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_RcvReportGoals::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */


INLINE lyra_id_t GMsg_RcvReportGoals::GoalID() const
{
  return data_.goalid;
}

INLINE void GMsg_RcvReportGoals::SetGoalID(lyra_id_t goalid)
{
  data_.goalid = goalid;
}
