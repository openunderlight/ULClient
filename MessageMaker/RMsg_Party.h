// RMsg_Party.h  -*- C++ -*-
// $Id: RMsg_Party.h,v 1.13 1998-05-01 17:05:12-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// party request/ack messages

#ifndef INCLUDED_RMsg_Party
#define INCLUDED_RMsg_Party

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"

// forward references

// message class

class RMsg_Party : public LmMesg {

public:

  enum request_enum {
    // request subtype
    UNKNOWN = 'U',  // invalid

    ACCEPT  = 'A',  // CS: leader accepts person; SC: join request accepted
    JOIN    = 'J',  // CS: ask leader to join party; SC: send leader join request
    LEAVE   = 'L',  // CS: leave party; SC: person left party
    REJECT  = 'R',  // CS: leader rejects person; SC: join request rejected

    // response codes
    RC_UNKNOWN = 1,         // invalid
    RC_UNUSED,              // unused (ACCEPT)
    // what kind of JOIN request being made
    JOIN_NORMAL,            // C   normal
    JOIN_AUTO,              // C   automatic
    // reason for JOIN request being rejected
    REJECT_NOTLEADER,       // S   target player is not party leader
    REJECT_PARTYFULL,       // C/S the party is full
    REJECT_MUSTLEAVEPARTY,  // S   already in party
    REJECT_NO,              // C   leader denied request
    REJECT_NOTINROOM,       // S   target player is not in room anymore
    REJECT_SELF,            // S   tried to join own party
    REJECT_BUSY,            // C/S target player has an outstanding join query
    REJECT_QUEUEFULL,       // C/S target player has maxed out requests
    REJECT_LEFT,            // S   new player left before accepting
    // leave types
    LEAVE_NORMAL,           // person left party "normally"
    LEAVE_LOGOUT            // person left party by logging out of level server

  };

public:

  RMsg_Party();
  ~RMsg_Party();

  void Init(int req_type, lyra_id_t playerid, int rc);

  // convenience functions
  void InitJoin(lyra_id_t playerid, int how);
  void InitAccept(lyra_id_t playerid);
  void InitReject(lyra_id_t playerid, int reason);
  void InitLeave(lyra_id_t playerid, int reason);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int RequestType() const;
  lyra_id_t PlayerID() const;
  int ResponseCode() const;

  // mutators
  void SetRequestType(int rt);
  void SetPlayerID(lyra_id_t playerid);
  void SetResponseCode(int rc);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    short req_type;      // request type
    short rc;            // response code (use depends on request type)
    lyra_id_t playerid;  // playerid (use depends on request type)
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_Party.i"
#endif

#endif /* INCLUDED_RMsg_Party */
