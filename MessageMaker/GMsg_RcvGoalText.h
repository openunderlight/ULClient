// GMsg_RcvGoalText.h  -*- C++ -*-
// $Id: GMsg_RcvGoalText.h,v 1.6 1997-08-18 18:00:57-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// receive goal text

#ifndef INCLUDED_GMsg_RcvGoalText
#define INCLUDED_GMsg_RcvGoalText

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_RcvGoalText : public LmMesg {

public:

  GMsg_RcvGoalText();
  ~GMsg_RcvGoalText();

  void Init(lyra_id_t goalid, short sugsphere, short sugstat, const TCHAR* creator, const TCHAR* goaltext);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t GoalID() const;
  short SugSphere() const;
  short SugStat() const;
  const TCHAR* Creator() const;
  const TCHAR* GoalText() const;

  // mutators
  void SetGoalID(lyra_id_t goalid);
  void SetSugSphere(short sugsphere);
  void SetSugStat(short sugstat);
  void SetCreator(const TCHAR* creator);
  void SetGoalText(const TCHAR* goaltext);

  // server-only members
  lyra_id_t CreatorID() const;
  void SetCreatorID(lyra_id_t creatorid);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual message size
  void calc_size();

  lyra_id_t creatorid_;

  // message data structure
  struct data_t {
    lyra_id_t goalid;
    short sugsphere;
    short sugstat;
    TCHAR creator[Lyra::PLAYERNAME_MAX];
    TCHAR goaltext[Lyra::MAX_GOAL_LENGTH];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_RcvGoalText.i"
#endif

#endif /* INCLUDED_GMsg_RcvGoalText */
