// GMsg_Goal.h  -*- C++ -*-
// $Id: GMsg_Goal.h,v 1.9 1997-11-20 16:26:40-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// generic goal-posting activity message

#ifndef INCLUDED_GMsg_Goal
#define INCLUDED_GMsg_Goal

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_Goal : public LmMesg {

public:

  enum {
    UNKNOWN = 0,

    // C->S requests          ID
    ACCEPT_GOAL,           // goalid
    REMOVE_GOAL,           // goalid
    VOTE_YES,              // goalid
    VOTE_NO,               // goalid
    EXPIRE_GOAL,           // goalid
    COMPLETE_GOAL,         // goalid
    GET_GOAL_TEXT,         // goalid
    GET_GOAL_DETAILS,      // goalid
    GET_REPORT_TEXT,       // reportid
    GET_GOALBOOK_HEADERS,  // none
    DELETE_REPORT,         // reportid
    GET_GUARDIAN_FLAG,     // goalid

    // S->C acks              ID
    DB_UNAVAILABLE,        // ?
    ACCEPT_GOAL_ACK,       // goalid
    ACCEPT_GOAL_ERROR,     // goalid
    VOTE_ACK,              // goalid
    VOTE_ERROR,            // goalid
    GOAL_NOTFOUND,         // goalid
    REPORT_NOTFOUND,       // reportid
    POSTGOAL_ACK,          // ?
    POSTGOAL_ERROR,        // ?
    POSTREPORT_ACK,        // ?
    POSTREPORT_ERROR,      // ?
    EXPIRE_GOAL_ACK,       // goalid
    EXPIRE_GOAL_ERROR,     // goalid
    COMPLETE_GOAL_ACK,     // goalid
    COMPLETE_GOAL_ERROR,   // goalid
    DELETE_REPORT_ACK,     // reportid
    DELETE_REPORT_ERROR,   // reportid
    GUARDIAN_FLAG_TRUE,    // goalid
    GUARDIAN_FLAG_FALSE    // goalid

  };

public:

  GMsg_Goal();
  ~GMsg_Goal();

  void Init(lyra_id_t id, int messagetyp);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t ID() const;
  int RequestType() const;

  // mutators
  void SetID(lyra_id_t id);
  void SetRequestType(int messagetyp);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t id;
    int messagetyp;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_Goal.i"
#endif

#endif /* INCLUDED_GMsg_Goal */
