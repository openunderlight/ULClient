// AMsg_ControlAgent.h  -*- C++ -*-
// $Id: AMsg_ControlAgent.h,v 1.2 1997-06-17 21:51:04-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// start/stop agent

#ifndef INCLUDED_AMsg_ControlAgent
#define INCLUDED_AMsg_ControlAgent

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "AMsg.h"

// forward references

// message class

class AMsg_ControlAgent : public LmMesg {

public:

  enum {
    // constants
    COMMAND_UNKNOWN = 'U',

    COMMAND_START		= 'G', // start an agent
    COMMAND_START_ALL   = 'A', // start all agents
    COMMAND_STOP		= 'R', // stop an agent
    COMMAND_POSESS		= 'P', // stop an agent
	COMMAND_ENABLE		= 'E', // enable agent updates to client
	COMMAND_DISABLE		= 'D', // disable agent updates to client
	COMMAND_KILL_DAEMON = 'K', // stop all agents & kill the daemon

    // default values
  };

public:

  AMsg_ControlAgent();
  ~AMsg_ControlAgent();

  void InitStart(lyra_id_t agent_id);
  void InitStop(lyra_id_t agent_id);

  void Init(lyra_id_t agent_id, int command);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t AgentID() const;
  int Command() const;

  // mutators
  void SetAgentID(lyra_id_t agent_id);
  void SetCommand(int command);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t agent_id;
    int command;
  } data_;

};

#ifdef USE_INLINE
#include "AMsg_ControlAgent.i"
#endif

#endif /* INCLUDED_AMsg_ControlAgent */
