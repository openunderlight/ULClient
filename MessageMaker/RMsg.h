// RMsg.h  -*- C++ -*-
// $Id: RMsg.h,v 1.22 1998-04-17 16:27:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// Room Server Message definitions

#ifndef INCLUDED_RMsg
#define INCLUDED_RMsg

#ifdef __GNUC__
#pragma interface
#endif

#include "LyraDefs.h"

////
//// Room Server messages
////

struct RMsg {

  // message type constants
  //
  // changing the order of this list breaks everything, so don't do it
  // no matter how tempting it might be to put things in order

  // key: CS = client->server; SC = server->client; SS = server<->server

  enum {

    NONE = Lyra::MSG_NONE, // invalid message type
    MIN = Lyra::RMSG_MIN,  // minimum known type (must be first)

    LOGOUT,                // CS    logout from (room) server
    GOTOROOM,              // CS    go to another room
    SPEECH,                // CS/SC speech (all types)
    LOGINACK,              // SC    login acknowledgement
    LEAVEROOM,             // SC    player left room
    ENTERROOM,             // SC    player(s) entered room
    ITEMDROP,              // SC    item(s) dropped (sent to other players)
    ITEMPICKUP,            // SC    item picked up (sent to other players)
    PARTY,                 // CS/SC party-related messages (join, reject, accept, leave)
    PARTYINFO,             // SC    party members

    JOINEDPARTY,           // SC    new person in party
    SERVERERROR,           // SC    server error
    ITEMHDRDROP,           // SC    item(s) dropped, header information
    PLAYERMSG,             // CS/SC send message to another player (skill trials, etc)
    PLAYERUPDATE,          // SC    peer update message
    UPDATE,                // CS    player update message
    PING,                  // CS/SC server ping
    CHANGEAVATAR,          // CS/SC player changed avatar
    GETAVATARDESCRIPTION,  // CS    request avatar description
    RCVAVATARDESCRIPTION,  // SC    avatar description info
    NEWLYAWAKENED,		   // SC    alert teachers/guardians/rulers to newlies
	CUPSUMMONS,			   // SC    alert when players enter the Cup Arena

	ROOMLOGINACK,		   // SC    sent to client on each room login; required to prevent use 
						   //		of aggressive arts, etc. before neighbors arrive

    // 24 total

    MAX                    // maximum known type (must be last, and less than
                           // LyraMessage::RMSG_MAX)
  };
};

#endif /* INCLUDED_RMsg */
