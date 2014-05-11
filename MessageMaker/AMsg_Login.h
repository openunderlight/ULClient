// AMsg_Login.h  -*- C++ -*-
// $Id: AMsg_Login.h,v 1.1 1997-06-17 15:34:01-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// login to agent server

#ifndef INCLUDED_AMsg_Login
#define INCLUDED_AMsg_Login

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "AMsg.h"

// forward references

// message class

class AMsg_Login : public LmMesg {

public:

  enum {
    // constants
    PLAYERNAME_MAX = Lyra::PLAYERNAME_MAX,
    PASSWORD_MAX = Lyra::PASSWORD_MAX,
  };

public:

  AMsg_Login();
  ~AMsg_Login();

  void Init(int version, const TCHAR* playername, const TCHAR* password);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Version() const;
  const TCHAR* PlayerName() const;
  const TCHAR* Password() const;

  // mutators
  void SetVersion(int version);
  void SetPlayerName(const TCHAR* playername);
  void SetPassword(const TCHAR* password);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int version;                      // client version
    TCHAR playername[PLAYERNAME_MAX];  // name
    TCHAR password[PASSWORD_MAX];      // password
  } data_;

};

#ifdef USE_INLINE
#include "AMsg_Login.i"
#endif

#endif /* INCLUDED_AMsg_Login */
