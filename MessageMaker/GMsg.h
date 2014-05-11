// GMsg.h  -*- C++ -*-
// $Id: GMsg.h,v 1.31 1998-04-24 16:18:04-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// Game Server Message definitions

#ifndef INCLUDED_GMsg
#define INCLUDED_GMsg

#ifdef __GNUC__
#pragma interface
#endif

#include "LyraDefs.h"

////
//// Game Server messages
////

struct GMsg {
  // message type constants
  //
  // changing the order of this list breaks everything, so don't do it
  // no matter how tempting it might be to put things in order

  // key: CS = client->server; SC = server->client; SS = server<->server

  enum {

    NONE = Lyra::MSG_MIN,  // invalid message type
    MIN = Lyra::GMSG_MIN,  // minimum known type (must be first)

    LOGIN,                 // CS    client login to game server
    LOGINACK,              // SC    server acknowledge login
    LOGOUT,                // CS    logout
    GETITEM,               // CS    get item from a room
    PUTITEM,               // CS    put item in a room
    ITEMDROP,              // SC    dropped item in a room
    ITEMPICKUP,            // SC    got item from room
    UPDATESTATS,           // CS    update player stats, + CS heartbeat
    UPDATEITEM,            // CS    update item in inventory (eg. after use)
    SERVERDOWN,            // SC    "server going down" message

    SERVERERROR,           // SC    server error
    CREATEITEM,            // CS    create item, put in inventory
    DESTROYITEM,           // CS    remove item from inventory
    GOTOLEVEL,             // CS    log in to level server
    CHANGESTAT,            // CS/SC change stat/skill
    GOAL,                  // CS/SC various goal-related uses (multi-message)
    GETGOALHDRS,           // CS    get goal board information
    GETREPORTHDRS,         // CS    get report header info
    POSTGOAL,              // CS    post a new goal
    POSTREPORT,            // CS    post a report to a goal

    RCVGOALDETAILS,        // SC    receive goal detail info
    RCVGOALHDR,            // SC    receive goal header
    RCVGOALTEXT,           // SC    receive goal text
    RCVREPORTHDR,          // SC    receive report header
    RCVREPORTTEXT,         // SC    receive report text
    DESTROYROOMITEM,       // CS    destroy an item in a room (without picking it up first)
    GETLEVELPLAYERS,       // CS    request # of players in level (by room)
    LEVELPLAYERS,          // SC    return # of players in level (by room)
    PING,                  // CS/SC ping server
    CHANGEAVATAR,          // CS/SC change player avatar (SC?)

    RCVGOALBOOKHDR,        // SC    receive goal header from goalbook
    CREATEPLAYER,          // CS    create player record
    CREATEPLAYERACK,       // SC    ack player creation
    GIVEITEM,              // CS    give item to another player
    GIVEITEMACK,           // SC    ack give item
    TAKEITEM,              // SC    take item from another player
    TAKEITEMACK,           // CS/SC ack take item
    LOCATEAVATAR,          // CS    locate given players
    AGENTLOGIN,            // CS    agent login
    LOCATEAVATARACK,       // SC    return player location

    GETITEMDESCRIPTION,    // CS    get item description
    ITEMDESCRIPTION,       // SC    return item description
    SHOWITEM,              // CS    show item to another player
    VIEWITEM,              // SC    view item from another player
    SETAVATARDESCRIPTION,  // CS    set avatar's description
    GETPLAYERNAME,         // CS    request player name
    RCVPLAYERNAME,         // SC    player name
    RCVREPORTGOALS,        // CS/SC receive list of goals player can get details/reports on
	SENSEDREAMERSACK,	   // SC    return dreamer locations
	LOCATENEWLIESACK,	   // SC    return newly awakened locations

	USEPPOINT,			   // SC    client trying to spend personality points
	GRANTPPOINT,		   // SC    client granting a ppoint to another
	PPOINTACK,			   // CS	response to above ppoint messages
	PRELOGIN,              // CS    client pre login notification to game server
    PRELOGINACK,           // SC    server acknowledge prelogin with timestamp & challenge
	LOCATEMARESACK,		   // SC    return newly awakened locations


    // 56 total

    MAX                    // maximum known type (must be last, and less than
                           // LyraMessage::GMSG_MAX)
  };
};

#endif /* INCLUDED_GMsg */
