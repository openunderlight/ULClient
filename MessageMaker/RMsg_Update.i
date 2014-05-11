// RMsg_Update.i  -*- C++ -*-
// $Id: RMsg_Update.i,v 1.5 1997-09-16 03:02:25-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_Update::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t RMsg_Update::PlayerID() const
{
  return data_.playerid;
}

INLINE const LmPeerUpdate& RMsg_Update::PeerUpdate() const
{
  return data_.update;
}

INLINE LmPeerUpdate& RMsg_Update::PeerUpdate()
{
  return data_.update;
}

INLINE void RMsg_Update::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void RMsg_Update::SetPeerUpdate(const LmPeerUpdate& update)
{
  data_.update = update;
}
