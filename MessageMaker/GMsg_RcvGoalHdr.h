// GMsg_RcvGoalHdr.h  -*- C++ -*-
// $Id: GMsg_RcvGoalHdr.h,v 1.6 1998-02-03 23:33:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// receive goal header

#ifndef INCLUDED_GMsg_RcvGoalHdr
#define INCLUDED_GMsg_RcvGoalHdr

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_RcvGoalHdr : public LmMesg {

public:

  GMsg_RcvGoalHdr();
  ~GMsg_RcvGoalHdr();

  void Init(lyra_id_t goalid, int sessionid, short status, short playeroption, const TCHAR* summary);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t GoalID() const;
  int SessionID() const;
  short Status() const;
  short PlayerOption() const;
  const TCHAR* Summary() const;

  // mutators
  void SetGoalID(lyra_id_t goalid);
  void SetSessionID(int sessionid);
  void SetStatus(short status);
  void SetPlayerOption(short playeroption);
  void SetSummary(const TCHAR* summary);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual message size
  void calc_size();

  // message data structure
  struct data_t {
    lyra_id_t goalid;
    int sessionid;
    short status;
    short playeroption; // did player vote on or accept this goal?
    TCHAR summary[Lyra::GOAL_SUMMARY_LENGTH];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_RcvGoalHdr.i"
#endif

#endif /* INCLUDED_GMsg_RcvGoalHdr */

