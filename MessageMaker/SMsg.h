// SMsg.h  -*- C++ -*-
// $Id: SMsg.h,v 1.35 1998-04-17 16:32:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// Inter-Server Message definitions

#ifndef INCLUDED_SMsg
#define INCLUDED_SMsg

#ifdef __GNUC__
#pragma interface
#endif

#include "LyraDefs.h"

struct SMsg {

  // Inter- and Intra-Server message type constants

  enum {
    NONE = Lyra::MSG_NONE, // invalid message type
    MIN = Lyra::SMSG_MIN,  // minimum known type (must be first)

    // inter-server messages
    LOGIN,                 // *S->*S server login
    LOGOUT,                // *S->*S server logout
    PING,                  // *S->*S server ping
    GETITEM,               // GS->LS remove item from room
    PUTITEM,               // GS->LS put item into room
    ITEMDROP,              // LS->GS item dropped
    ITEMPICKUP,            // LS->GS item picked up
    SERVERERROR,           // *S->*S generic server error
    PROXY,                 // *S->*S process message from proxied client, forward to client, etc.
    LEVELLOGIN,            // GS->LS level server player login

    DESTROYROOMITEM,       // GS->LS destroy item in room, no pickup
    GETLEVELPLAYERS,       // GS->LS get # of players in level, by room
    GETSERVERSTATUS,       // MS->*S request player/server status
    SERVERSTATUS,          // *S->MS return server status
    PLAYERSTATUS,          // *S->MS return player status
    CONNSTATUS,            // *S->MS return connection status
    GIVEITEM,              // *S->*S give item to a player
    GIVEITEMACK,           // *S->*S ack give item
    TAKEITEMACK,           // *S->*S ack take item from a player
    LOCATEAVATAR,          // *S->*S locate player, return room

    PARTYLEADER,           // LS->GS indicates player was a party leader for some amount of time
    SHOWITEM,              // GS->LS show item to another player
    GASPLOGIN,             // gS->GS player logging into gs from gasp
    GASPLOGOUT,            // gS->GS player logging out from gasp
    SETAVATARDESCRIPTION,  // GS->LS change player's avatar description

    // any server internal messages
    RECVMESG,              // message thread: receive message from given connection
    SENDMESG,              // network output thread: send message to given connection

    // level server internal messages
    LS_LOGIN,              // new player logged in
    LS_ACTION,             // perform some action

    // game server internal messages
    GS_ACTION,             // perform some action
    GS_LOGIN,              // player login
    GS_LOGOUT,             // player logout

    // gasp internal messages
    US_ACTION,             // perform some action

    MAX                    // maximum known type (must be last)
  };
};

#endif /* INCLUDED_SMsg */
