// GMsg_RcvPlayerName.i  -*- C++ -*-
// $Id: GMsg_RcvPlayerName.i,v 1.1 1998-04-24 16:18:04-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_RcvPlayerName::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_RcvPlayerName::PlayerID() const
{
  return data_.playerid;
}

INLINE int GMsg_RcvPlayerName::RequestID() const
{
  return data_.requestid;
}

INLINE const TCHAR* GMsg_RcvPlayerName::PlayerName() const
{
  return data_.playername;
}

INLINE void GMsg_RcvPlayerName::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void GMsg_RcvPlayerName::SetRequestID(int requestid)
{
  data_.requestid = requestid;
}
