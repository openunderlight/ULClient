// GMsg_GetLevelPlayers.h  -*- C++ -*-
// $Id: GMsg_GetLevelPlayers.h,v 1.3 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// request number of players in a given level, by room

#ifndef INCLUDED_GMsg_GetLevelPlayers
#define INCLUDED_GMsg_GetLevelPlayers

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_GetLevelPlayers : public LmMesg {

public:

  GMsg_GetLevelPlayers();
  ~GMsg_GetLevelPlayers();

  void Init(lyra_id_t levelid);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors/mutators
  lyra_id_t LevelID() const;
  void SetLevelID(lyra_id_t levelid);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t levelid;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_GetLevelPlayers.i"
#endif

#endif /* INCLUDED_GMsg_GetLevelPlayers */
