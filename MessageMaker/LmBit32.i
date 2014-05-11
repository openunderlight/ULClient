// LmBit32.i  -*- C++ -*-
// $Id: LmBit32.i,v 1.5 1997-07-15 15:06:49-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions

INLINE LmBit32::LmBit32(unsigned int x)
{
  x_ = x;
}

INLINE unsigned int LmBit32::Value() const
{
  return x_;
}

INLINE unsigned int LmBit32::BitMask(unsigned int len)
{
  return (1 << len) - 1;  // 2^len - 1
}

INLINE unsigned int LmBit32::BitMask(unsigned int start, unsigned int len)
{
  return BitMask(len) << start;
}

INLINE unsigned int LmBit32::GetBits(unsigned int start, unsigned int len) const
{
  return (x_ >> start) & BitMask(len);
}

INLINE void LmBit32::SetValue(unsigned int value)
{
  x_ = value;
}

INLINE void LmBit32::ClearBits(unsigned int start, unsigned int len)
{
  x_ &= ~BitMask(start, len);
}

INLINE void LmBit32::SetBits(unsigned int start, unsigned int len, unsigned int val)
{
  val &= BitMask(len);    // chop val down to size
  val = val << start;     // shift up to start position
  ClearBits(start, len);  // clear part corresponding to val
  x_ |= val;              // set bits
}
