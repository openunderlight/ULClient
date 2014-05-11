// LmArts.i  -*- C++ -*-
// $Id: LmArts.i,v 1.2 1997-07-30 16:45:42-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void LmArts::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */


