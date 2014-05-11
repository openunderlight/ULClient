// AMsg_PosessInfo.h  -*- C++ -*-
// $Id: AMsg_PosessInfo.h,v 1.2 1997-06-18 20:17:27-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// generic server PosessInfo message

#ifndef INCLUDED_AMsg_PosessInfo
#define INCLUDED_AMsg_PosessInfo

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "AMsg.h"

// forward references

// message class

class AMsg_PosessInfo : public LmMesg {

public:

 enum {
    OK = 1,  // posession succeeded
    FAILED = 2
  };

public:

  AMsg_PosessInfo();
  ~AMsg_PosessInfo();

  void Init(int status);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  TCHAR* Name();
  TCHAR* Password();
  int	Status(void);

  // mutators
  void SetName(TCHAR* value);
  void SetPassword(TCHAR* value);
  void SetStatus(int value);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
	   int	status;
	   TCHAR username[Lyra::PLAYERNAME_MAX];
	   TCHAR password[Lyra::PASSWORD_MAX];
  } data_;

};

#ifdef USE_INLINE
#include "AMsg_PosessInfo.i"
#endif

#endif /* INCLUDED_AMsg_PosessInfo */
