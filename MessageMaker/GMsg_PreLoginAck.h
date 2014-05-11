// GMsg_PreLoginAck.h  -*- C++ -*-
// $Id: GMsg_PreLoginAck.h,v 1.21 1998-04-03 17:00:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// PreLogin acknowledge message

#ifndef INCLUDED_GMsg_PreLoginAck
#define INCLUDED_GMsg_PreLoginAck

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_PreLoginAck : public LmMesg {
public:

	enum {
		PRELOGIN_OK        = 'O',  // prelogin OK'd, challenge issued
		PRELOGIN_UNKNOWNERROR = 'E',  // unknown server error
		PRELOGIN_GAMEFULL     = 'F'  // game is full
	};


  GMsg_PreLoginAck();
  ~GMsg_PreLoginAck();

  void Init(int version, int status, const TCHAR* challenge);
	  
  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Version() const;
  int Status() const;
  const TCHAR* Challenge() const;
  
  // mutators
  void SetVersion(int version);
  void SetStatus(int status);
  void SetChallenge(const TCHAR* description);


public: // logically private

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int version;
    int status; // NOTE: we use the GMsg_LoginAck status codes here too
    TCHAR challenge[Lyra::CHALLENGE_LENGTH];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_PreLoginAck.i"
#endif

#endif /* INCLUDED_GMsg_PreLoginAck */
