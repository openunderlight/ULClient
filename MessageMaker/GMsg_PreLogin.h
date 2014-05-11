// GMsg_PreLogin.h  -*- C++ -*-
// $Id: GMsg_PreLogin.h,v 1.14 1998-04-17 16:37:44-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// PreLogin message

#ifndef INCLUDED_GMsg_PreLogin
#define INCLUDED_GMsg_PreLogin

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_PreLogin : public LmMesg {

public:

  enum {
    // default values
    DEFAULT_VERSION = 0
  };

public:

  GMsg_PreLogin();
  ~GMsg_PreLogin();

  void Init(int version);
	  
  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Version() const;

  // mutators
  void SetVersion(int version);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int version;                            // client version
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_PreLogin.i"
#endif

#endif /* INCLUDED_GMsg_PreLogin */
