// GMsg_UpdateStats.h  -*- C++ -*-
// $Id: GMsg_UpdateStats.h,v 1.10 1997-07-29 19:32:10-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// update player stats message

#ifndef INCLUDED_GMsg_UpdateStats
#define INCLUDED_GMsg_UpdateStats

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"
#include "LmStats.h"
#include "LmArts.h"

// forward references

// message class

class GMsg_UpdateStats : public LmMesg {

public:

  GMsg_UpdateStats();
  ~GMsg_UpdateStats();

  void Init(const LmStats& stats, const LmArts& arts);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  const LmStats& PlayerStats() const;
  const LmArts& Arts() const;

  // mutators
  void SetPlayerStats(const LmStats& stats);
  void SetArts(const LmArts& arts);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    LmStats stats;
    LmArts arts;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_UpdateStats.i"
#endif

#endif /* INCLUDED_GMsg_UpdateStats */
