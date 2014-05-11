// RMsg_RoomLoginAck.h  -*- C++ -*-
// $Id: RMsg_RoomLoginAck.h,v 1.17 1997-11-17 14:05:06-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// login acknowledgement

#ifndef INCLUDED_RMsg_RoomLoginAck
#define INCLUDED_RMsg_RoomLoginAck

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"

// forward references

// message class

class RMsg_RoomLoginAck : public LmMesg {

public:

  enum {
    // ack status
    LOGIN_UNKNOWN        = 'U',

	LOGIN_ERROR			 = 'E',
    LOGIN_ROOMFULL       = 'F',
    LOGIN_OK             = 'K',
    LOGIN_ROOMNOTFOUND   = 'R'
  };

public:

  RMsg_RoomLoginAck();
  ~RMsg_RoomLoginAck();

  void Init(short status, short num_neighbors);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  short Status() const;
  short NumNeighbors() const;

  //mutators
  void SetStatus(short status);
  void SetNumNeighbors(short num_neighbors);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    short status;
	short num_neighbors;
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_RoomLoginAck.i"
#endif

#endif /* INCLUDED_RMsg_RoomLoginAck */
