// GMsg_PPointAck.h  -*- C++ -*-
// $Id: GMsg_PPointAck.h,v 1.1 1997-09-19 16:42:16-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// acknowledge item giver

#ifndef INCLUDED_GMsg_PPointAck
#define INCLUDED_GMsg_PPointAck

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"
#include "LmItemHdr.h"

// forward references

// message class

class GMsg_PPointAck : public LmMesg {

public:

	enum { // what kind of ack this is
		GRANT_ACK = 1,
		USE_ACK = 2
	};

  enum {  // server->client responses
    GRANT_OK		= 'G',
	GRANT_REPEAT	= 'R', // rejected cuz a grant was made to same target within a month
	USE_OK			= 'U',	// use of personality point approved
	USE_NOT_ENOUGH	= 'N', // not enough pp's to activate desired function
	UNKNOWN_ERR		= 'E', // unknown error
	USE_STAT_MAX	= 'M', // trying to increase maxed out stat
	USE_ART_MAX		= 'A', // trying to plat maxed out art
	USE_CANT_TRAIN	= 'T', // can't train - maybe not max level before plat?
	USE_CANT_SPHERE	= 'S', // can't sphere - maybe not ready?

  };

public:

  GMsg_PPointAck();
  ~GMsg_PPointAck();

  void Init(int type, int result); //, int var1, int var2);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  unsigned char Type() const;
  unsigned char Result() const;
  //int Var1() const;
  //int Var2() const;

  // mutators
  void SetType(unsigned char value);
  void SetResult(unsigned char value);
//  void SetVar1(int var1);
//  void SetVar2(int var2);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    unsigned char type;
	unsigned char result;
    unsigned char padding[2];
	//int var1;
	//int var2;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_PPointAck.i"
#endif

#endif /* INCLUDED_GMsg_PPointAck */
