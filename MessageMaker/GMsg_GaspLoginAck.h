// GMsg_GaspLoginAck.h  -*- C++ -*-
// $Id: GMsg_GaspLoginAck.h,v 1.1 1998-01-22 15:08:27-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// gasp login acknowledge message

#ifndef INCLUDED_GMsg_GaspLoginAck
#define INCLUDED_GMsg_GaspLoginAck

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"
#include "LmItem.h"
#include "LmStats.h"
#include "LmAvatar.h"
#include "LmArts.h"

// forward references

// message class

class GMsg_GaspLoginAck : public LmMesg {

public:

  enum {
    // login status values
    LOGIN_UNUSED       = 'U',  // invalid

    LOGIN_UNKNOWNERROR = 'E',  // unknown server error
    LOGIN_GAMEFULL     = 'F',  // game is full
    LOGIN_OK           = 'O',  // successful login
    LOGIN_NOTALLOWED   = 'T',  // player is not allowed in game
    LOGIN_WRONGVERSION = 'V',  // incorrect client version

    // default values
    DEFAULT_VERSION = 0
  };

public:

  GMsg_GaspLoginAck();
  ~GMsg_GaspLoginAck();

  void Init(int version, int status, int server_port);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Version() const;
  int Build() const;
  int SubBuild() const;
  int Status() const;
  int ServerPort() const;

  // mutators
  void SetVersion(int version);
  void SetStatus(int status);
  void SetServerPort(int server_port);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int version;
    int request_status;
    int server_port;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_GaspLoginAck.i"
#endif

#endif /* INCLUDED_GMsg_GaspLoginAck */
