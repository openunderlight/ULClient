// GMsg_PostReport.h  -*- C++ -*-
// $Id: GMsg_PostReport.h,v 1.5 1997-08-06 00:41:49-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// post a goal report

#ifndef INCLUDED_GMsg_PostReport
#define INCLUDED_GMsg_PostReport

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_PostReport : public LmMesg {

public:

  GMsg_PostReport();
  ~GMsg_PostReport();

  void Init(lyra_id_t goalid, int awardxp, const TCHAR* recipient, const TCHAR* summary, const TCHAR* reporttext);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t GoalID() const;
  int AwardXP() const;
  const TCHAR* Recipient() const;
  const TCHAR* Summary() const;
  const TCHAR* ReportText() const;

  // mutators
  void SetGoalID(lyra_id_t goalid);
  void SetAwardXP(int awardxp);
  void SetRecipient(const TCHAR* recipient);
  void SetSummary(const TCHAR* summary);
  void SetReportText(const TCHAR* reporttext);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual message size
  void calc_size();

  // message data structure
  struct data_t {
    lyra_id_t goalid;
    int awardxp;
    TCHAR recipient[Lyra::PLAYERNAME_MAX];
    TCHAR summary[Lyra::GOAL_SUMMARY_LENGTH];
    TCHAR reporttext[Lyra::MAX_REPORT_LENGTH];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_PostReport.i"
#endif

#endif /* INCLUDED_GMsg_PostReport */


