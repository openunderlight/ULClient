// RMsg_PartyInfo.h  -*- C++ -*-
// $Id: RMsg_PartyInfo.h,v 1.14 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// party members message

#ifndef INCLUDED_RMsg_PartyInfo
#define INCLUDED_RMsg_PartyInfo

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

class RMsg_PartyInfo : public LmMesg {

public:

  enum {
    // default values
    DEFAULT_PARTYSIZE = 0
  };

public:

  RMsg_PartyInfo();
  ~RMsg_PartyInfo();

  void Init(lyra_id_t leaderid, int psize);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t LeaderID() const;
  int PartySize() const;
  const RmRemotePlayer& PartyMember(int num) const;

  // mutators
  void SetLeaderID(lyra_id_t leaderid);
  void SetPartySize(int num);
  void SetPartyMember(int num, const RmRemotePlayer& rp);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  void calc_size();
  void calc_players();

  int num_members_;

  // message data structure
  struct data_t {
    lyra_id_t leaderid;
    RmRemotePlayer members[Lyra::MAX_PARTYSIZE];
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_PartyInfo.i"
#endif

#endif /* INCLUDED_RMsg_PartyInfo */
