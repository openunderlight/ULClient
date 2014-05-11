// GMsg_UsePPoint.i  -*- C++ -*-
// $Id: GMsg_UsePPoint.i,v 1.3 1997-11-25 15:53:47-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_UsePPoint::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE short GMsg_UsePPoint::How() const
{
  return data_.how;
}

INLINE short GMsg_UsePPoint::Var1() const
{
  return data_.var1;
}

INLINE short GMsg_UsePPoint::Var2() const
{
  return data_.var2;
}

INLINE short GMsg_UsePPoint::Var3() const
{
  return data_.var3;
}

INLINE void GMsg_UsePPoint::SetHow(short value)
{
  data_.how = value;
}

INLINE void GMsg_UsePPoint::SetVar1(short value)
{
  data_.var1 = value;
}

INLINE void GMsg_UsePPoint::SetVar2(short value)
{
  data_.var2 = value;
}

INLINE void GMsg_UsePPoint::SetVar3(short value)
{
  data_.var3 = value;
}

