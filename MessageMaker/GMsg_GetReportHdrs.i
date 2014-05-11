// GMsg_GetReportHdrs.i  -*- C++ -*-
// $Id: GMsg_GetReportHdrs.i,v 1.6 1998-02-04 19:03:35-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_GetReportHdrs::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_GetReportHdrs::GoalID() const
{
  return data_.goalid;
}

INLINE short GMsg_GetReportHdrs::Guild() const
{
  return data_.guild;
}

INLINE short GMsg_GetReportHdrs::LevelNum() const
{
  return data_.level_num;
}

INLINE int GMsg_GetReportHdrs::SessionID() const
{
  return data_.sessionid;
}

INLINE lyra_id_t GMsg_GetReportHdrs::LastReport() const
{
  return data_.lastreport;
}

INLINE void GMsg_GetReportHdrs::SetGoalID(lyra_id_t goalid)
{
  data_.goalid = goalid;
}

INLINE void GMsg_GetReportHdrs::SetGuild(short guild)
{
  data_.guild = guild;
}

INLINE void GMsg_GetReportHdrs::SetLevelNum(short level_num)
{
  data_.level_num = level_num;
}

INLINE void GMsg_GetReportHdrs::SetSessionID(int sessionid)
{
  data_.sessionid = sessionid;
}

INLINE void GMsg_GetReportHdrs::SetLastReport(lyra_id_t lastreport)
{
  data_.lastreport = lastreport;
}
