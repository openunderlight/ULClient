// LmParty.h  -*- C++ -*-
// $Id: LmParty.h,v 1.6 1997-07-09 19:24:52-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// "character party" class

#ifndef INCLUDED_LmParty
#define INCLUDED_LmParty

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"

// forward declarations

// class declarations

class LmParty {

public:

  enum {
    // constants
    MAX_PARTYSIZE = Lyra::MAX_PARTYSIZE,

    // defaults
    DEFAULT_LEADER = Lyra::ID_UNKNOWN,
    DEFAULT_ID = Lyra::ID_UNKNOWN
  };

public:

  LmParty();

  void Create(lyra_id_t leaderid);
  int RemovePlayer(lyra_id_t playerid);
  int AddPlayer(lyra_id_t playerid);
  void Empty();

  int PartySize() const;
  int MaxPartySize() const;
  bool IsEmpty() const;
  bool IsFull() const;
  bool HasPlayer(lyra_id_t playerid) const;
  lyra_id_t LeaderID() const;
  lyra_id_t PlayerID(int index) const;

  void Dump(FILE* f, int indent = 0) const;
  void Dump1(FILE* f) const;

private:

  int index_of(lyra_id_t playerid) const;

  lyra_id_t leaderid_;
  int num_members_;
  lyra_id_t members_[MAX_PARTYSIZE];

};

#ifdef USE_INLINE
#include "LmParty.i"
#endif

#endif /* INCLUDED_LmParty */
