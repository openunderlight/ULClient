// GMsg_GetLevelPlayers.i  -*- C++ -*-
// $Id: GMsg_GetLevelPlayers.i,v 1.2 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_GetLevelPlayers::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_GetLevelPlayers::LevelID() const
{
  return data_.levelid;
}

INLINE void GMsg_GetLevelPlayers::SetLevelID(lyra_id_t levelid)
{
  data_.levelid = levelid;
}
