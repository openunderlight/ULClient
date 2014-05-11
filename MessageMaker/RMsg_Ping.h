// RMsg_Ping.h  -*- C++ -*-
// $Id: RMsg_Ping.h,v 1.2 1997-07-18 17:26:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// level server ping message

#ifndef INCLUDED_RMsg_Ping
#define INCLUDED_RMsg_Ping

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"

// forward references

// message class

class RMsg_Ping : public LmMesg {

public:

  RMsg_Ping();
  ~RMsg_Ping();

  void Init(int nonce);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors/mutators
  int Nonce() const;
  void SetNonce(int nonce);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int nonce;
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_Ping.i"
#endif

#endif /* INCLUDED_RMsg_Ping */
