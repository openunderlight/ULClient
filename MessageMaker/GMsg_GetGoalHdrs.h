/// ***

// GMsg_GetGoalHdrs.h  -*- C++ -*-
// $Id: GMsg_GetGoalHdrs.h,v 1.5 1998-02-03 23:33:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// get goal headers for a given guild/level

#ifndef INCLUDED_GMsg_GetGoalHdrs
#define INCLUDED_GMsg_GetGoalHdrs

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_GetGoalHdrs : public LmMesg {

public:

  GMsg_GetGoalHdrs();
  ~GMsg_GetGoalHdrs();

  void Init(int sessionid, short guild, short level_num, lyra_id_t lastgoal);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int SessionID() const;
  short Guild() const;
  short LevelNum() const;
  lyra_id_t LastGoal() const;

  // mutators
  void SetSessionID(int sessionid);
  void SetGuild(short guild);
  void SetLevelNum(short level_num);
  void SetLastGoal(lyra_id_t lastgoal);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int sessionid;
    short guild;
    short level_num;
    lyra_id_t lastgoal;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_GetGoalHdrs.i"
#endif

#endif /* INCLUDED_GMsg_GetGoalHdrs */
