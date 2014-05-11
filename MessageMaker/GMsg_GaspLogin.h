// GMsg_GaspLogin.h  -*- C++ -*-
// $Id: GMsg_GaspLogin.h,v 1.1 1998-01-22 15:08:27-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// gasp login message

#ifndef INCLUDED_GMsg_GaspLogin
#define INCLUDED_GMsg_GaspLogin

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_GaspLogin : public LmMesg {

public:

  enum {
    // default values
    DEFAULT_VERSION = 0
  };

public:

  GMsg_GaspLogin();
  ~GMsg_GaspLogin();

  void Init(int version, mpath_id_t mpathid, int subversion);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Version() const;
  int SubVersion() const;
  mpath_id_t MPathID() const;

  // mutators
  void SetVersion(int version);
  void SetSubVersion(int subversion);
  void SetMPathID(mpath_id_t mpathid);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int version;                            // client version
    mpath_id_t mpathid;                     // user's MPATH id
    int subversion;                         // another int to use for version checking
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_GaspLogin.i"
#endif

#endif /* INCLUDED_GMsg_GaspLogin */
