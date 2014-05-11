// GMsg_GetGoalHdrs.i  -*- C++ -*-
// $Id: GMsg_GetGoalHdrs.i,v 1.6 1998-02-03 23:33:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_GetGoalHdrs::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_GetGoalHdrs::SessionID() const
{
  return data_.sessionid;
}

INLINE short GMsg_GetGoalHdrs::Guild() const
{
  return data_.guild;
}

INLINE short GMsg_GetGoalHdrs::LevelNum() const
{
  return data_.level_num;
}

INLINE lyra_id_t GMsg_GetGoalHdrs::LastGoal() const
{
  return data_.lastgoal;
}

INLINE void GMsg_GetGoalHdrs::SetSessionID(int sessionid)
{
  data_.sessionid = sessionid;
}

INLINE void GMsg_GetGoalHdrs::SetGuild(short guild)
{
  data_.guild = guild;
}

INLINE void GMsg_GetGoalHdrs::SetLevelNum(short level_num)
{
  data_.level_num = level_num;
}

INLINE void GMsg_GetGoalHdrs::SetLastGoal(lyra_id_t lastgoal)
{
  data_.lastgoal = lastgoal;
}
