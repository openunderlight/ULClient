// GMsg_RcvReportHdr.i  -*- C++ -*-
// $Id: GMsg_RcvReportHdr.i,v 1.5 1998-02-03 23:33:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_RcvReportHdr::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_RcvReportHdr::ReportID() const
{
  return data_.reportid;
}

INLINE lyra_id_t GMsg_RcvReportHdr::GoalID() const
{
  return data_.goalid;
}

INLINE short GMsg_RcvReportHdr::SessionID() const
{
  return data_.sessionid;
}

INLINE unsigned short GMsg_RcvReportHdr::Flags() const
{
  return data_.flags;
}

INLINE const TCHAR* GMsg_RcvReportHdr::Summary() const
{
  return data_.summary;
}

INLINE void GMsg_RcvReportHdr::SetReportID(lyra_id_t reportid)
{
  data_.reportid = reportid;
}

INLINE void GMsg_RcvReportHdr::SetGoalID(lyra_id_t goalid)
{
  data_.goalid = goalid;
}

INLINE void GMsg_RcvReportHdr::SetSessionID(short sessionid)
{
  data_.sessionid = sessionid;
}

INLINE void GMsg_RcvReportHdr::SetFlags(unsigned short flags)
{
  data_.flags = flags;
}

