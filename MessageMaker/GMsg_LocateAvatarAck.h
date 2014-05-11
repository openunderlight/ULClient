// GMsg_LocateAvatarAck.h  -*- C++ -*-
// $Id: GMsg_LocateAvatarAck.h,v 1.3 1998-02-26 14:35:44-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// locate player

#ifndef INCLUDED_GMsg_LocateAvatarAck
#define INCLUDED_GMsg_LocateAvatarAck

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_LocateAvatarAck : public LmMesg {

public:

  enum {
    // constants
    MAX_PLAYERS = 12,  // *note* must be same in GMsg_LocateAvatar.h!

    // locate status values
    LOCATE_AGENT          = 'A',  // player is an agent, cannot locate!
    LOCATE_FOUND          = 'F',  // player is in game, location is given
    LOCATE_FOUND_HIDDEN   = 'M',  // player is mindblinked, but gm found anyway
    LOCATE_HIDDEN         = 'H',  // player is in game, but hidden
    LOCATE_NOTLOGGEDIN    = 'N',  // player not logged in game
    LOCATE_PLAYERNOTFOUND = 'P',  // player not in database
    LOCATE_UNKNOWN        = 'U'   // unknown status
  };

public:

  GMsg_LocateAvatarAck();
  ~GMsg_LocateAvatarAck();

  void Init(int num_players);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int NumPlayers() const;
  const TCHAR* PlayerName(int player_num) const;
  int Status(int player_num) const;
  lyra_id_t LevelID(int player_num) const;
  lyra_id_t RoomID(int player_num) const;

  // mutators
  void SetNumPlayers(int num_players);
  void SetPlayerName(int player_num, const TCHAR* playername);
  void SetStatus(int player_num, int status);
  void SetLocation(int player_num, lyra_id_t levelid, lyra_id_t roomid);

public: // logically private

  // player info structure
  struct pinfo_t {
    TCHAR playername[Lyra::PLAYERNAME_MAX];
    int status;
    lyra_id_t levelid;
    lyra_id_t roomid;
  };

private:

  // standard non-public methods
  void hton();
  void ntoh();

  void calc_size();
  void calc_players();

  // non-message data
  int num_players_;

  // message data structure
  struct data_t {
    pinfo_t players[MAX_PLAYERS];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_LocateAvatarAck.i"
#endif

#endif /* INCLUDED_GMsg_LocateAvatarAck */
