// GMsg_UsePPoint.h  -*- C++ -*-
// $Id: GMsg_UsePPoint.h,v 1.4 1997-12-05 14:44:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// locate player request

#ifndef INCLUDED_GMsg_UsePPoint
#define INCLUDED_GMsg_UsePPoint

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_UsePPoint : public LmMesg {

public:

  enum {
	STAT_INCREASE = 0,
	BUY_PMARE_TIME = 1,
	USE_ART = 2,
	GAIN_XP = 3,

  };

public:

  GMsg_UsePPoint();
  ~GMsg_UsePPoint();

  void Init(short how, short var1, short var2, short var3);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  short How() const;
  short Var1() const;
  short Var2() const;
  short Var3() const;

  // mutators
  void SetHow(short how);
  void SetVar1(short var1);
  void SetVar2(short var2);
  void SetVar3(short var3);

public: // logically private

  // player name type
  typedef TCHAR pname_t[Lyra::PLAYERNAME_MAX];

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
	  short how;
	  short var1;
	  short var2;
	  short var3;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_UsePPoint.i"
#endif

#endif /* INCLUDED_GMsg_UsePPoint */
