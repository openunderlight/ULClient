// GMsg_SummonAvatar.i  -*- C++ -*-
// $Id: GMsg_SummonAvatar.i,v 1.3 1997-11-25 15:53:47-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_SummonAvatar::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_SummonAvatar::NumPlayers() const
{
  return num_players_;
}

INLINE const char* GMsg_SummonAvatar::PlayerName(int player_num) const
{
  return data_.players[player_num];
}
