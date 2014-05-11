// GMsg_GrantPPoint.h  -*- C++ -*-
// $Id: GMsg_GrantPPoint.h,v 1.4 1997-12-05 14:44:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// locate player request

#ifndef INCLUDED_GMsg_GrantPPoint
#define INCLUDED_GMsg_GrantPPoint

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_GrantPPoint : public LmMesg {
public:
	enum {
		MAX_WHY = 200 // escaped it must be <255, so this is reasonable
	};

public:

  GMsg_GrantPPoint();
  ~GMsg_GrantPPoint();

  void Init(lyra_id_t player_id, const TCHAR* why);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  const TCHAR* Why(void) const;

  // mutators
  void SetPlayerID(lyra_id_t value);
  void SetWhy(const TCHAR* why);

private:

  // standard non-public methods
  void hton();
  void ntoh();
  void calc_size();

  // message data structure
  struct data_t {
	lyra_id_t	player_id;
    TCHAR		why[MAX_WHY];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_GrantPPoint.i"
#endif

#endif /* INCLUDED_GMsg_GrantPPoint */
