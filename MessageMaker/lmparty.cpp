// LmParty.cpp  -*- C++ -*-
// $Id: LmParty.cpp,v 1.4 1997-07-18 16:04:59-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmParty.h"
#endif

#include <stdio.h>

#include "LmParty.h"
#include "LyraDefs.h"

#ifndef USE_INLINE
#include "LmParty.i"
#endif

////
// Constructor
////

LmParty::LmParty()
{
  Empty();
}

////
// Create
////

void LmParty::Create(lyra_id_t leaderid)
{
  Empty();
  leaderid_ = leaderid;
  AddPlayer(leaderid);
}

////
// Empty: empty party
////

void LmParty::Empty()
{
  for (int i = 0; i < MAX_PARTYSIZE; ++i) {
    members_[i] = DEFAULT_ID;
  }
  leaderid_ = DEFAULT_LEADER;
  num_members_ = 0;
}

////
// AddPlayer: add player to party; return 0 if successful, -1 otherwise
////

int LmParty::AddPlayer(lyra_id_t playerid)
{
  if (IsFull() || (index_of(playerid) != -1)) {
    return -1;
  }
  // add at end
  members_[num_members_] = playerid;
  ++num_members_;
  return 0;
}

////
// RemovePlayer: remove player from party; return 0 if successful, -1 otherwise
////

int LmParty::RemovePlayer(lyra_id_t playerid)
{
  int idx = index_of(playerid);
  if (idx == -1) {
    return -1;
  }
  // remove by swapping with last item
  num_members_--;
  members_[idx] = members_[num_members_];
  // clear out last item (necessary if only one element)
  members_[num_members_] = DEFAULT_ID;
  // check if party is now empty (ie. current player is only member) and if so, clear
  if (num_members_ == 1) {
    Empty();
    return 0;
  }
  // check if leadership will change
  // if party leader left, new leader is remaining player with smallest player id
  if (leaderid_ == playerid) {
    lyra_id_t newleader = members_[0];
    for (int i = 1; i < num_members_; ++i) {
      if (members_[i] < newleader) {
        newleader = members_[i];
      }
    }
    leaderid_ = newleader;
  }
  return 0;
}

////
// Dump
////

#ifdef USE_DEBUG
void LmParty::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
  fprintf(f, "<LmParty[%p]: leader=%u members(%d)=[", this, LeaderID(), PartySize());
  for (int i = 0; i < PartySize(); ++i) {
    fprintf(f, "%u", PlayerID(i));
    if (i != (PartySize() - 1)) {
      fprintf(f, ",");
    }
  }
  fprintf(f, "]>\n");
}
#endif /* USE_DEBUG */

////
// Dump1
////

#ifdef USE_DEBUG
void LmParty::Dump1(FILE* f) const
{
  if (IsEmpty()) {
    fprintf(f, "[]");
  }
  else {
    fprintf(f, "[%u:", LeaderID());
    for (int i = 0; i < PartySize(); ++i) {
      fprintf(f, "%u", PlayerID(i));
      if (i != (PartySize() - 1)) {
	fprintf(f, ",");
      }
    }
    fprintf(f, "]");
  }
}
#endif /* USE_DEBUG */

////
// index_of: return index into members_ for given playerid, or -1 if not found
////

int LmParty::index_of(lyra_id_t playerid) const
{
  for (int i = 0; i < MAX_PARTYSIZE; ++i) {
    if (members_[i] == playerid) {
      return i;
    }
  }
  return -1;
}
