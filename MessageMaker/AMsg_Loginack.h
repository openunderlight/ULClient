// AMsg_LoginAck.h  -*- C++ -*-
// $Id: AMsg_LoginAck.h,v 1.2 1997-06-17 21:51:04-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// login acknowledge message

#ifndef INCLUDED_AMsg_LoginAck
#define INCLUDED_AMsg_LoginAck

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "AMsg.h"

struct agent_id_t 
{
  TCHAR		name[Lyra::PLAYERNAME_MAX];
  lyra_id_t id;
  lyra_id_t level_id;
  int		type;
};


// forward references

// message class

class AMsg_LoginAck : public LmMesg {

public:

  enum {
    // login status values
    LOGIN_UNUSED       = 'U',  // invalid
    LOGIN_UNKNOWNERROR = 'E',  // unknown server error
    LOGIN_USERNOTFOUND = 'N',  // user not in database
    LOGIN_OK           = 'O',  // successful login
    LOGIN_BADPASSWORD  = 'P',  // password incorrect
    LOGIN_WRONGVERSION = 'V',  // incorrect client version

    // default values
    DEFAULT_VERSION = 0,
  };

public:

  AMsg_LoginAck();
  ~AMsg_LoginAck();

  void Init(int version, int status, int num_agents);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Version() const;
  int Status() const;
  int Build() const;
  int SubBuild() const;
  int NumAgents() const;
  const agent_id_t& ID(int index) const;

  // calculate actual size
  void calc_size();

  // mutators
  void SetVersion(int version);
  void SetStatus(int status);
  void SetNumAgents(int num_agents);
  void SetID(int index, const agent_id_t& id);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    short version;
    short request_status;
	int	  num_agents;
	agent_id_t ids[AMsg::MAX_AGENTS];
  } data_;

};

#ifdef USE_INLINE
#include "AMsg_LoginAck.i"
#endif

#endif /* INCLUDED_AMsg_LoginAck */
