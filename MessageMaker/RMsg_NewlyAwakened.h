// RMsg_NewlyAwakened.h  -*- C++ -*-
// $Id: RMsg_NewlyAwakened.h,v 1.3 1998-02-26 14:35:44-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// locate player

#ifndef INCLUDED_RMsg_NewlyAwakened
#define INCLUDED_RMsg_NewlyAwakened

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class RMsg_NewlyAwakened : public LmMesg {

public:

  enum {
    // constants
    MAX_PLAYERS = 10
  };

public:

  RMsg_NewlyAwakened();
  ~RMsg_NewlyAwakened();

  void Init();

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  const TCHAR* PlayerName(void) const;
  lyra_id_t RoomID(void) const;

  // mutators
  void SetPlayerName(const TCHAR* playername);
  void SetLocation(lyra_id_t roomid);

public: // logically private

private:

  // standard non-public methods
  void hton();
  void ntoh();


  // message data structure
  struct data_t {
    TCHAR playername[Lyra::PLAYERNAME_MAX];
    lyra_id_t roomid;
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_NewlyAwakened.i"
#endif

#endif /* INCLUDED_RMsg_NewlyAwakened */
