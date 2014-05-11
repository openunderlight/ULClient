// RMsg_EnterRoom.i  -*- C++ -*-
// $Id: RMsg_EnterRoom.i,v 1.5 1997-07-18 17:26:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_EnterRoom::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int RMsg_EnterRoom::NumPlayers() const
{
  return num_players_;
}

INLINE const RmRemotePlayer& RMsg_EnterRoom::Player(int num) const
{
  return data_.players[num];
}

INLINE void RMsg_EnterRoom::SetPlayer(int num, const RmRemotePlayer& p)
{
  data_.players[num] = p;
}
