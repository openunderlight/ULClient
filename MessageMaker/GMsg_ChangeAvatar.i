// GMsg_ChangeAvatar.i  -*- C++ -*-
// $Id: GMsg_ChangeAvatar.i,v 1.5 1997-07-28 21:59:02-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_ChangeAvatar::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE const LmAvatar& GMsg_ChangeAvatar::Avatar() const
{
  return data_.avatar;
}

INLINE int GMsg_ChangeAvatar::Which() const
{
  return data_.which;
}

INLINE void GMsg_ChangeAvatar::SetAvatar(const LmAvatar& avatar)
{
  data_.avatar = avatar;
}

INLINE void GMsg_ChangeAvatar::SetWhich(int which)
{
  data_.which = which;
}
