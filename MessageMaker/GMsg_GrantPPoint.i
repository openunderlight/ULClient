// GMsg_GrantPPoint.i  -*- C++ -*-
// $Id: GMsg_GrantPPoint.i,v 1.3 1997-11-25 15:53:47-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_GrantPPoint::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_GrantPPoint::PlayerID() const
{
  return data_.player_id;
}

INLINE void GMsg_GrantPPoint::SetPlayerID(lyra_id_t value)
{
  data_.player_id = value;
}

INLINE const TCHAR* GMsg_GrantPPoint::Why(void) const
{
	return data_.why;
}

