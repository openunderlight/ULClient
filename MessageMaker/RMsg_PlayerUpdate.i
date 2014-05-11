// RMsg_PlayerUpdate.i  -*- C++ -*-
// $Id: RMsg_PlayerUpdate.i,v 1.6 1997-07-18 17:26:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_PlayerUpdate::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t RMsg_PlayerUpdate::PlayerID() const
{
  return data_.playerid;
}

INLINE int RMsg_PlayerUpdate::NumPlayers() const
{
  return num_players_;
}

INLINE const LmPeerUpdate& RMsg_PlayerUpdate::PeerUpdate(int num) const
{
  return data_.players[num];
}

INLINE void RMsg_PlayerUpdate::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void RMsg_PlayerUpdate::SetPeerUpdate(int num, const LmPeerUpdate& update)
{
  data_.players[num] = update;
}
