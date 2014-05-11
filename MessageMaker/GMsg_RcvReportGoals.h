// GMsg_RcvReportGoals.h  -*- C++ -*-
// $Id: GMsg_RcvReportGoals.h,v 1.6 1998-02-03 23:33:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// receive report header

#ifndef INCLUDED_GMsg_RcvReportGoals
#define INCLUDED_GMsg_RcvReportGoals

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_RcvReportGoals : public LmMesg {

public:

  GMsg_RcvReportGoals();
  ~GMsg_RcvReportGoals();

  void Init(lyra_id_t goalid);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t GoalID() const;

  // mutators
  void SetGoalID(lyra_id_t goalid);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t goalid;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_RcvReportGoals.i"
#endif

#endif /* INCLUDED_GMsg_RcvReportGoals */



