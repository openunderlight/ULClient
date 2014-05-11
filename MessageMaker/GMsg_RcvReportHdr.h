// GMsg_RcvReportHdr.h  -*- C++ -*-
// $Id: GMsg_RcvReportHdr.h,v 1.6 1998-02-03 23:33:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// receive report header

#ifndef INCLUDED_GMsg_RcvReportHdr
#define INCLUDED_GMsg_RcvReportHdr

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_RcvReportHdr : public LmMesg {

public:

  GMsg_RcvReportHdr();
  ~GMsg_RcvReportHdr();

  void Init(lyra_id_t reportid, lyra_id_t goalid, short sessionid, const TCHAR* summary, unsigned short flags);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t ReportID() const;
  lyra_id_t GoalID() const;
  short SessionID() const;
  unsigned short Flags() const;
  const TCHAR* Summary() const;

  // mutators
  void SetReportID(lyra_id_t reportid);
  void SetGoalID(lyra_id_t goalid);
  void SetFlags(unsigned short flags);
  void SetSessionID(short sessionid);
  void SetSummary(const TCHAR* summary);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual message size
  void calc_size();

  // message data structure
  struct data_t {
    lyra_id_t reportid;
    lyra_id_t goalid;
    short sessionid;
    unsigned short flags;
    TCHAR summary[Lyra::GOAL_SUMMARY_LENGTH];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_RcvReportHdr.i"
#endif

#endif /* INCLUDED_GMsg_RcvReportHdr */
