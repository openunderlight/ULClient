// GMsg_GetPlayerName.i  -*- C++ -*-
// $Id: GMsg_GetPlayerName.i,v 1.1 1998-04-24 16:18:04-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_GetPlayerName::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_GetPlayerName::PlayerID() const
{
  return data_.playerid;
}

INLINE int GMsg_GetPlayerName::RequestID() const
{
  return data_.requestid;
}

INLINE void GMsg_GetPlayerName::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void GMsg_GetPlayerName::SetRequestID(int requestid)
{
  data_.requestid = requestid;
}
