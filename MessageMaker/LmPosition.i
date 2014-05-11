// LmPosition.i  -*- C++ -*-
// $Id: LmPosition.i,v 1.5 1997-07-18 16:04:59-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void LmPosition::Dump(FILE*, int) const
{
  // empty
}

INLINE void LmPosition::Dump1(FILE*) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE short LmPosition::X() const
{
  return pos_.x;
}

INLINE short LmPosition::Y() const
{
  return pos_.y;
}

INLINE short LmPosition::Height() const
{
  return pos_.height;
}

INLINE unsigned short LmPosition::Angle() const
{
  return pos_.angle;
}

INLINE void LmPosition::SetX(short x)
{
  pos_.x = x;
}

INLINE void LmPosition::SetY(short y)
{
  pos_.y = y;
}

INLINE void LmPosition::SetHeight(short height)
{
  pos_.height = height;
}

INLINE void LmPosition::SetAngle(unsigned short angle)
{
  pos_.angle = angle;
}
