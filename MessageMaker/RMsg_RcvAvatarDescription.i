// RMsg_RcvAvatarDescription.i  -*- C++ -*-
// $Id: RMsg_RcvAvatarDescription.i,v 1.1 1998-04-17 16:27:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_RcvAvatarDescription::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t RMsg_RcvAvatarDescription::PlayerID() const
{
  return data_.playerid;
}

INLINE const TCHAR* RMsg_RcvAvatarDescription::Description() const
{
  return data_.description;
}

INLINE void RMsg_RcvAvatarDescription::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}
