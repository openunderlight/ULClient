// GMsg_RcvReportText.h  -*- C++ -*-
// $Id: GMsg_RcvReportText.h,v 1.7 1998-02-03 23:33:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// receive report text

#ifndef INCLUDED_GMsg_RcvReportText
#define INCLUDED_GMsg_RcvReportText

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_RcvReportText : public LmMesg {

public:

  GMsg_RcvReportText();
  ~GMsg_RcvReportText();

  void Init(lyra_id_t reportid, lyra_id_t goalid, int awardxp, int flags, const TCHAR* creator, const TCHAR* recipient,
	    const TCHAR* reporttext);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t ReportID() const;
  lyra_id_t GoalID() const;
  int AwardXP() const;
  int Flags() const;
  const TCHAR* Creator() const;
  const TCHAR* Recipient() const;
  const TCHAR* ReportText() const;

  // mutators
  void SetReportID(lyra_id_t reportid);
  void SetGoalID(lyra_id_t goalid);
  void SetAwardXP(int awardxp);
  void SetFlags(int flags);
  void SetCreator(const TCHAR* creator);
  void SetRecipient(const TCHAR* recipient);
  void SetReportText(const TCHAR* reporttext);

  // server-only
  lyra_id_t CreatorID() const;
  lyra_id_t RecipientID() const;
  void SetCreatorID(lyra_id_t creatorid);
  void SetRecipientID(lyra_id_t recipientid);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual message size
  void calc_size();

  lyra_id_t creatorid_;
  lyra_id_t recipientid_;

  // message data structure
  struct data_t {
    lyra_id_t reportid;
    lyra_id_t goalid;
    int awardxp;
    int flags;
    TCHAR creator[Lyra::PLAYERNAME_MAX];
    TCHAR recipient[Lyra::PLAYERNAME_MAX];
    TCHAR reporttext[Lyra::MAX_REPORT_LENGTH];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_RcvReportText.i"
#endif

#endif /* INCLUDED_GMsg_RcvReportText */

