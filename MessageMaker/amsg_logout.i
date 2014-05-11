// AMsg_Logout.i  -*- C++ -*-
// $Id: AMsg_Logout.i,v 1.1 1997-06-17 15:34:01-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void AMsg_Logout::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */
