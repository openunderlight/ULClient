// RMsg_ChangeAvatar.i  -*- C++ -*-
// $Id: RMsg_ChangeAvatar.i,v 1.4 1997-07-28 21:59:12-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_ChangeAvatar::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t RMsg_ChangeAvatar::PlayerID() const
{
  return data_.playerid;
}

INLINE const LmAvatar& RMsg_ChangeAvatar::Avatar() const
{
  return data_.avatar;
}

INLINE void RMsg_ChangeAvatar::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void RMsg_ChangeAvatar::SetAvatar(const LmAvatar& avatar)
{
  data_.avatar = avatar;
}
