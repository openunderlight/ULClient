// GMsg_RcvGoalHdr.i  -*- C++ -*-
// $Id: GMsg_RcvGoalHdr.i,v 1.5 1998-02-03 23:33:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_RcvGoalHdr::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_RcvGoalHdr::GoalID() const
{
  return data_.goalid;
}

INLINE int GMsg_RcvGoalHdr::SessionID() const
{
  return data_.sessionid;
}

INLINE short GMsg_RcvGoalHdr::Status() const
{
  return data_.status;
}

INLINE short GMsg_RcvGoalHdr::PlayerOption() const
{
  return data_.playeroption;
}

INLINE const TCHAR* GMsg_RcvGoalHdr::Summary() const
{
  return data_.summary;
}

INLINE void GMsg_RcvGoalHdr::SetGoalID(lyra_id_t goalid)
{
  data_.goalid = goalid;
}

INLINE void GMsg_RcvGoalHdr::SetSessionID(int sessionid)
{
  data_.sessionid = sessionid;
}

INLINE void GMsg_RcvGoalHdr::SetStatus(short status)
{
  data_.status = status;
}

INLINE void GMsg_RcvGoalHdr::SetPlayerOption(short playeroption)
{
  data_.playeroption = playeroption;
}
