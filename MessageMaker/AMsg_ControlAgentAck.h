// AMsg_ControlAgentAck.h  -*- C++ -*-
// $Id: AMsg_ControlAgentAck.h,v 1.1 1997-06-17 15:34:01-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// ack to controlagent message

#ifndef INCLUDED_AMsg_ControlAgentAck
#define INCLUDED_AMsg_ControlAgentAck

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "AMsg.h"

// forward references

// message class

class AMsg_ControlAgentAck : public LmMesg {

public:

  enum {
    // constants
    STATUS_UNKNOWN = 'U',
		// general acks
	D_UNKNOWN = 'I',

		// acks for starting
	ALREADY_RUNNING = 'A',
	AGENT_STARTED = 'S',

		// acks for stopping
	NOT_RUNNING = 'N',
	AGENT_STOPPED = 'T',

    // default values
  };

public:

  AMsg_ControlAgentAck();
  ~AMsg_ControlAgentAck();

  void Init(lyra_id_t agent_id, int status);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t AgentID() const;
  int Status() const;

  // mutators
  void SetAgentID(lyra_id_t agent_id);
  void SetStatus(int status);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t agent_id;
    int status;
  } data_;

};

#ifdef USE_INLINE
#include "AMsg_ControlAgentAck.i"
#endif

#endif /* INCLUDED_AMsg_ControlAgentAck */
