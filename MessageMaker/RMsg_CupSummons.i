// RMsg_CupSummons.i  -*- C++ -*-
// $Id: RMsg_CupSummons.i,v 1.1 1997-11-25 15:53:47-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_CupSummons::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */


INLINE const TCHAR* RMsg_CupSummons::PlayerName(void) const
{
  return data_.playername;
}


