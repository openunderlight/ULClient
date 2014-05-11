// AMsg_Logout.h  -*- C++ -*-
// $Id: AMsg_Logout.h,v 1.1 1997-06-17 15:34:01-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// logout message

#ifndef INCLUDED_AMsg_Logout
#define INCLUDED_AMsg_Logout

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "AMsg.h"

// forward references

// message class

class AMsg_Logout : public LmMesg {

public:

  AMsg_Logout();
  ~AMsg_Logout();

  void Init();

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors/mutators: none

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int dummy;  // have to have some data, but it's not used (yet)
  } data_;

};

#ifdef USE_INLINE
#include "AMsg_Logout.i"
#endif

#endif /* INCLUDED_AMsg_Logout */
