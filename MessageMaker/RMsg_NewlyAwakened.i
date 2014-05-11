// RMsg_NewlyAwakened.i  -*- C++ -*-
// $Id: RMsg_NewlyAwakened.i,v 1.1 1997-11-25 15:53:47-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_NewlyAwakened::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */


INLINE const TCHAR* RMsg_NewlyAwakened::PlayerName(void) const
{
  return data_.playername;
}

INLINE lyra_id_t RMsg_NewlyAwakened::RoomID(void) const
{
  return data_.roomid;
}


INLINE void RMsg_NewlyAwakened::SetLocation(lyra_id_t roomid)
{
  data_.roomid = roomid;
}

