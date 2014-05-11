// RmRemotePlayer.h  -*- C++ -*-
// $Id: RmRemotePlayer.h,v 1.9 1997-07-28 21:59:12-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// remote player information

#ifndef INCLUDED_RmRemotePlayer
#define INCLUDED_RmRemotePlayer

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "RMsg.h"
#include "LmPeerUpdate.h"
#include "LmAvatar.h"

// forward declarations

// base datatypes

struct rmsg_remoteplayer_t {
  LmPeerUpdate update; // contains (per-room) real time ID
  lyra_id_t playerid; // full 4 byte player id
  LmAvatar avatar;
  lyra_id_t room;
  TCHAR playername[Lyra::PLAYERNAME_MAX];
};

// the wrapper class

class RmRemotePlayer {

public:

  // default instance
  static const RmRemotePlayer DEFAULT_INSTANCE;

public:

  RmRemotePlayer();

  void Init(const LmPeerUpdate& update, const LmAvatar& avatar, 
	  const TCHAR* playername, lyra_id_t playerid, lyra_id_t room); 

  // selectors
  lyra_id_t PlayerID() const; 
  lyra_id_t Room() const;  
  const LmAvatar& Avatar() const;
  const LmPeerUpdate& PeerUpdate() const;
  const TCHAR* PlayerName() const;

  // mutators
  void SetRoom(lyra_id_t room); 
  void SetPlayerID(lyra_id_t playerid); 
  void SetAvatar(const LmAvatar& avatar);
  void SetPeerUpdate(const LmPeerUpdate& update);
  void SetPlayerName(const TCHAR* playername);

  // byteorder conversions
  void ConvertToNetwork();
  void ConvertToHost();

  // output
  void Dump(FILE* f, int indent = 0) const;

private:

  rmsg_remoteplayer_t rp_;

};

#ifdef USE_INLINE
#include "RmRemotePlayer.i"
#endif

#endif /* INCLUDED_RmRemotePlayer */

