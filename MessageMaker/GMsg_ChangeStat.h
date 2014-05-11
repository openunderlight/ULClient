// GMsg_ChangeStat.h  -*- C++ -*-
// $Id: GMsg_ChangeStat.h,v 1.6 1997-09-23 02:32:26-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// change stat request, ack

#ifndef INCLUDED_GMsg_ChangeStat
#define INCLUDED_GMsg_ChangeStat

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_ChangeStat : public LmMesg {

public:

  // Generally speaking, a CS message is a request; until the server responds with
  // the appropriate SET_ message, the client's change is not realized.

  enum {
    MAX_CHANGES = 100,  // max number of changes that can be put into one message

    REQ_UNKNOWN = 0,
    STAT_UNKNOWN = -1,

    // request type    dir   stat  value    result
    SET_XP = 1,     // SC    N/A   XP       set XP
    SET_STAT_CURR,  // CS    stat  statval  set current player stat
    SET_STAT_MAX,   // SC    stat  statval  set max player stat
    SET_SKILL       // SC/CS art   skill    set skill
    // SET_GUILD       // SC    guild rank     set player's guild rank

    // stat/art constants, guild values, ranks, etc: see SharedConstants.h
  };

public:

  GMsg_ChangeStat();
  ~GMsg_ChangeStat();

  void Init(int num_changes);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int NumChanges() const;
  int RequestType(int change_num) const;
  int Stat(int change_num) const;
  int Value(int change_num) const;

  // mutators
  void InitChange(int change_num, int requesttype, int stat, int value);
  void SetNumChanges(int num_changes);
  void SetRequestType(int change_num, int requesttype);
  void SetStat(int change_num, int stat);
  void SetValue(int change_num, int value);

public: // logically private

  struct change_t {
    short requesttype;
    short stat;
    int value;
  };

private:

  // standard non-public methods
  void hton();
  void ntoh();

  void calc_size();
  void calc_changes();

  // check array index
  static bool change_num_ok(int change_num);

  int num_changes_; // number of changes in data_.changes

  // message data structure
  struct data_t {
    change_t changes[MAX_CHANGES];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_ChangeStat.i"
#endif

#endif /* INCLUDED_GMsg_ChangeStat */
