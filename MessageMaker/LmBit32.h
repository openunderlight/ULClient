// LmBit32.h  -*- C++ -*-
// $Id: LmBit32.h,v 1.4 1997-07-15 15:06:49-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// a 32-bit wide bitfield

#ifndef INCLUDED_LmBit32
#define INCLUDED_LmBit32

#ifdef __GNUC__
#pragma interface
#endif

#include "LyraDefs.h"

// class declarations

class LmBit32 {

public:

  LmBit32(unsigned int x = 0);

  // selectors
  unsigned int Value() const;
  unsigned int GetBits(unsigned int start, unsigned int len) const;

  // mutators
  void SetValue(unsigned int val);
  void SetBits(unsigned int start, unsigned int len, unsigned int val);
  void ClearBits(unsigned int start, unsigned int len);

  // static helper methods
  static unsigned int BitMask(unsigned int len);
  static unsigned int BitMask(unsigned int start, unsigned int len);

  // byteorder conversions
  void ConvertToHost();
  void ConvertToNetwork();

  // convert to a string
  void UnParse(char* str, int len) const;

private:

  unsigned int x_;

};

#ifdef USE_INLINE
#include "LmBit32.i"
#endif

// inline functions

inline int operator==(const LmBit32& left, const LmBit32& right)
{
  return left.Value() == right.Value();
}

#endif /* INCLUDED_LmBit32 */
