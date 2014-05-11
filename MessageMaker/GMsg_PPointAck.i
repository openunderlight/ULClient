// GMsg_PPointAck.i  -*- C++ -*-
// $Id: GMsg_PPointAck.i,v 1.1 1997-09-19 16:42:16-07 jason Exp $
// Copyright 2002 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_PPointAck::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE unsigned char GMsg_PPointAck::Type() const
{
  return data_.type;
}

INLINE unsigned char GMsg_PPointAck::Result() const
{
  return data_.result;
}
/*
INLINE int GMsg_PPointAck::Var1() const
{
  return data_.var1;
}

INLINE int GMsg_PPointAck::Var2() const
{
  return data_.var2;
}


INLINE void GMsg_PPointAck::SetVar1(int value)
{
  data_.var1 = value;
}

INLINE void GMsg_PPointAck::SetVar2(int value)
{
  data_.var2 = value;
}
*/
INLINE void GMsg_PPointAck::SetType(unsigned char value)
{
  data_.type = value;
}

INLINE void GMsg_PPointAck::SetResult(unsigned char value)
{
  data_.result = value;
}
