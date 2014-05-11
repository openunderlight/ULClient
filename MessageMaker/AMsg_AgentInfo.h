// AMsg_AgentInfo.h  -*- C++ -*-
// $Id: AMsg_AgentInfo.h,v 1.1 1997-06-17 15:34:01-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// message class template

#ifndef INCLUDED_AMsg_AgentInfo
#define INCLUDED_AMsg_AgentInfo

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "AMsg.h"

// forward references

// message class

class AMsg_AgentInfo : public LmMesg {

public:

  enum {
    // agent status values
	STATUS_NONE = 0,		  // no agent exists
    STATUS_READY,             // ready to run
    STATUS_RUNNING,			  // running
    STATUS_ABORTED,           // agent aborted by itself
    STATUS_CRASHED,           // daemon detected agent crash or hang
    STATUS_POSESSED,          // agent has been posessed by a gm
	STATUS_IN_FLUX,			  // agent is being created or destroyed
  };

public:

  AMsg_AgentInfo();
  ~AMsg_AgentInfo();

  void Init(lyra_id_t agent_id, short kills, short deaths, unsigned char frame_rate, unsigned char status,
			unsigned char busy, unsigned char room_id, short x, short y);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t AgentID() const;
  short Kills() const;
  short Deaths() const;
  unsigned char FrameRate() const;
  unsigned char Status() const;
  char Busy() const; // neg value means alone; pos means now busy
  unsigned char RoomID() const;
  short X() const;
  short Y() const;

  // mutators
  void SetAgentID(lyra_id_t agent_id);
  void SetKills(short kills);
  void SetDeaths(short deaths);
  void SetFrameRate(unsigned char frame_rate);
  void SetStatus(unsigned char status);
  void SetBusy(char percent);
  void SetRoomID(unsigned char room_id);
  void SetX(short x);
  void SetY(short y);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t agent_id;
	short kills;
	short deaths;
	unsigned char frame_rate;
    unsigned char status;
	unsigned char busy;
	unsigned char room_id;
	short x;
	short y;
  } data_;

};

#ifdef USE_INLINE
#include "AMsg_AgentInfo.i"
#endif

#endif /* INCLUDED_AMsg_AgentInfo */
