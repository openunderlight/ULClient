// RMsg_JoinedParty.h  -*- C++ -*-
// $Id: RMsg_JoinedParty.h,v 1.9 1997-07-18 17:26:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// new party member

#ifndef INCLUDED_RMsg_JoinedParty
#define INCLUDED_RMsg_JoinedParty

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"
#include "RmRemotePlayer.h"

// forward references

// message class

class RMsg_JoinedParty : public LmMesg {

public:

  RMsg_JoinedParty();
  ~RMsg_JoinedParty();

  void Init(const RmRemotePlayer& player);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors/mutators
  const RmRemotePlayer& PartyMember() const;
  void SetPartyMember(const RmRemotePlayer& member);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    RmRemotePlayer member;
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_JoinedParty.i"
#endif

#endif /* INCLUDED_RMsg_JoinedParty */
