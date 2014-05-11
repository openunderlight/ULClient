// GMsg_RcvGoalbookHdr.h  -*- C++ -*-
// $Id: GMsg_RcvGoalbookHdr.h,v 1.1 1997-08-18 22:00:58-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// receive goal header

#ifndef INCLUDED_GMsg_RcvGoalbookHdr
#define INCLUDED_GMsg_RcvGoalbookHdr

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_RcvGoalbookHdr : public LmMesg {

public:

  GMsg_RcvGoalbookHdr();
  ~GMsg_RcvGoalbookHdr();

  void Init(lyra_id_t goalid, short guild, short rank, const TCHAR* summary);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t GoalID() const;
  short Guild() const;
  short Rank() const;
  const TCHAR* Summary() const;

  // mutators
  void SetGoalID(lyra_id_t goalid);
  void SetGuild(short guild);
  void SetRank(short rank);
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
    short guild;
    short rank;
    TCHAR summary[Lyra::GOAL_SUMMARY_LENGTH];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_RcvGoalbookHdr.i"
#endif

#endif /* INCLUDED_GMsg_RcvGoalbookHdr */
