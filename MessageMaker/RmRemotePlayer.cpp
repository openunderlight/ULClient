// RmRemotePlayer.cpp  -*- C++ -*-
// $Id: RmRemotePlayer.cpp,v 1.10 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "RmRemotePlayer.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock2.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif /* WIN32 */
#include <stdio.h>
#include <string.h>

#include "RmRemotePlayer.h"

#ifndef USE_INLINE
#include "RmRemotePlayer.i"
#endif

// default instance
const RmRemotePlayer RmRemotePlayer::DEFAULT_INSTANCE;

////
// constructor
////

RmRemotePlayer::RmRemotePlayer()
{
  // initialize to defaults
	Init(LmPeerUpdate::DEFAULT_INSTANCE, LmAvatar::DEFAULT_INSTANCE, _T("player"), Lyra::ID_UNKNOWN, 0);
}

////
// Init
////

void RmRemotePlayer::Init(const LmPeerUpdate& update, const LmAvatar& avatar, const TCHAR* playername,
						lyra_id_t playerid, lyra_id_t room) 
{
  SetPeerUpdate(update);
  SetAvatar(avatar);
  SetPlayerName(playername);
  SetPlayerID(playerid);
  SetRoom(room);
}

////
// SetPlayerName
////

void RmRemotePlayer::SetPlayerName(const TCHAR* playername)
{
 _tcsnccpy(rp_.playername, playername, sizeof(rp_.playername));
  TRUNC(rp_.playername, sizeof(rp_.playername));
}

////
// ConvertToNetwork
///

void RmRemotePlayer::ConvertToNetwork()
{
  rp_.avatar.ConvertToNetwork();
  rp_.update.ConvertToNetwork();
  // no conversion: playername
}

////
// ConvertToHost
////

void RmRemotePlayer::ConvertToHost()
{
  rp_.avatar.ConvertToHost();
  rp_.update.ConvertToHost();
  // no conversion: playername
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RmRemotePlayer::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RmRemotePlayer[%p,%d]: player=%u name='%s'>\n"), this, sizeof(RmRemotePlayer),
	  PlayerID(), PlayerName());
  Avatar().Dump(f, indent + 1);
  PeerUpdate().Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
