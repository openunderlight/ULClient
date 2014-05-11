// GMsg_SetAvatarDescription.i  -*- C++ -*-
// $Id: GMsg_SetAvatarDescription.i,v 1.1 1998-04-17 16:25:53-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_SetAvatarDescription::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE const TCHAR* GMsg_SetAvatarDescription::Description() const
{
  return data_.description;
}
