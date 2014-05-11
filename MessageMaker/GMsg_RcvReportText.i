// GMsg_RcvReportText.i  -*- C++ -*-
// $Id: GMsg_RcvReportText.i,v 1.6 1998-02-03 23:33:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_RcvReportText::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_RcvReportText::ReportID() const
{
  return data_.reportid;
}

INLINE lyra_id_t GMsg_RcvReportText::GoalID() const
{
  return data_.goalid;
}

INLINE int GMsg_RcvReportText::AwardXP() const
{
  return data_.awardxp;
}

INLINE int GMsg_RcvReportText::Flags() const
{
  return data_.flags;
}

INLINE const TCHAR* GMsg_RcvReportText::Creator() const
{
  return data_.creator;
}

INLINE const TCHAR* GMsg_RcvReportText::Recipient() const
{
  return data_.recipient;
}

INLINE const TCHAR* GMsg_RcvReportText::ReportText() const
{
  return data_.reporttext;
}

INLINE void GMsg_RcvReportText::SetReportID(lyra_id_t reportid)
{
  data_.reportid = reportid;
}

INLINE void GMsg_RcvReportText::SetGoalID(lyra_id_t goalid)
{
  data_.goalid = goalid;
}

INLINE void GMsg_RcvReportText::SetAwardXP(int awardxp)
{
  data_.awardxp = awardxp;
}

INLINE void GMsg_RcvReportText::SetFlags(int flags)
{
  data_.flags = flags;
}

INLINE lyra_id_t GMsg_RcvReportText::CreatorID() const
{
  return creatorid_;
}

INLINE lyra_id_t GMsg_RcvReportText::RecipientID() const
{
  return recipientid_;
}

INLINE void GMsg_RcvReportText::SetCreatorID(lyra_id_t creatorid)
{
  creatorid_ = creatorid;
}

INLINE void GMsg_RcvReportText::SetRecipientID(lyra_id_t recipientid)
{
  recipientid_ = recipientid;
}

