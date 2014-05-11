// GMsg_GetReportHdrs.h  -*- C++ -*-
// $Id: GMsg_GetReportHdrs.h,v 1.5 1998-02-04 19:03:35-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// get report headers for a given goal

#ifndef INCLUDED_GMsg_GetReportHdrs
#define INCLUDED_GMsg_GetReportHdrs

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_GetReportHdrs : public LmMesg {

public:

  GMsg_GetReportHdrs();
  ~GMsg_GetReportHdrs();

  void Init(lyra_id_t goalid, short guild, short level_num, int sessionid, lyra_id_t lastreport);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t GoalID() const;
  int SessionID() const;
  short Guild() const;
  short LevelNum() const;  
  lyra_id_t LastReport() const;

  // mutators
  void SetGoalID(lyra_id_t goalid);
  void SetGuild(short guild);
  void SetLevelNum(short level_num);
  void SetSessionID(int sessionid);
  void SetLastReport(lyra_id_t lastreport);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t goalid;
    short guild;
    short level_num;
    int sessionid;
    lyra_id_t lastreport;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_GetReportHdrs.i"
#endif

#endif /* INCLUDED_GMsg_GetReportHdrs */
