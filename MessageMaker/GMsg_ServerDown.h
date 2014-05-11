// GMsg_ServerDown.h  -*- C++ -*-
// $Id: GMsg_ServerDown.h,v 1.8 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// server outage message

#ifndef INCLUDED_GMsg_ServerDown
#define INCLUDED_GMsg_ServerDown

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_ServerDown : public LmMesg {

public:

  enum {
    // constants
    STATUS_UNKNOWN  = 'U',  // invalid status
    STATUS_SHUTDOWN = 'S'   // server shutdown 
  };

public:

  GMsg_ServerDown();
  ~GMsg_ServerDown();

  void Init(int status);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors/mutators
  int Status() const;
  void SetStatus(int status);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int status;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_ServerDown.i"
#endif

#endif /* INCLUDED_GMsg_ServerDown */
