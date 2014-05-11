// RMsg_CupSummons.h  -*- C++ -*-
// $Id: RMsg_CupSummons.h,v 1.3 1998-02-26 14:35:44-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// locate player

#ifndef INCLUDED_RMsg_CupSummons
#define INCLUDED_RMsg_CupSummons

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class RMsg_CupSummons : public LmMesg {

public:


public:

  RMsg_CupSummons();
  ~RMsg_CupSummons();

  void Init();

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  const TCHAR* PlayerName(void) const;

  // mutators
  void SetPlayerName(const TCHAR* playername);

public: // logically private

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    TCHAR playername[Lyra::PLAYERNAME_MAX];
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_CupSummons.i"
#endif

#endif /* INCLUDED_RMsg_CupSummons */
