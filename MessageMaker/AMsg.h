// AMsg.h  -*- C++ -*-
// $Id: AMsg.h,v 1.3 1997-06-18 15:39:42-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// Agent Server Message definitions

#ifndef INCLUDED_AMsg
#define INCLUDED_AMsg

#ifdef __GNUC__
#pragma interface
#endif

#include "LyraDefs.h"

////
//// Game Server messages
////

struct AMsg {

  // message type constants
  //
  // changing the order of this list breaks everything, so don't do it
  // no matter how tempting it might be to put things in order

  // key: CS = client->server; SC = server->client; SS = server<->server

  enum {

    NONE = Lyra::MSG_MIN,  // invalid message type
    MIN = Lyra::AMSG_MIN,  // minimum known type (must be first)

    LOGIN,                 // CS    client login to game server
    LOGINACK,              // SC    server acknowledge login
    LOGOUT,                // CS    logout
    POSESSINFO,            // SC    posession info
    AGENTINFO,             // SC
    CONTROLAGENT,          // CS
    CONTROLAGENTACK,       // SC

    MAX                    // maximum known type (must be last, and less than
                           // LyraMessage::AMSG_MAX)
  };

  enum {
	  AGENT_DAEMON_PORT = 9900,
	  MAX_AGENTS = 256	// max agents available per process

  };

};

#endif /* INCLUDED_AMsg */
