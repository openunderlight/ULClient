// LmPosition.h  -*- C++ -*-
// $Id: LmPosition.h,v 1.18 1997-07-18 16:04:59-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// position class (replaces position_t)

#ifndef INCLUDED_LmPosition
#define INCLUDED_LmPosition

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"

// forward declarations

// base datatype

struct lyra_position_t {
  short x;
  short y;
  short height;
  unsigned short angle;
};

// the class

class LmPosition {

public:

  // constants
  enum {
    DEFAULT_X = 0,
    DEFAULT_Y = 0,
    DEFAULT_HEIGHT = 0,
    DEFAULT_ANGLE = 0
  };

  // default instance
  static const LmPosition DEFAULT_INSTANCE;

public:

  LmPosition();

  // re-construct
  void Init(short x, short y, short height, unsigned short angle);

  // HACK: return base object
  lyra_position_t& HACKGETSTRUCT() { return pos_; }

  // selectors
  short X() const;
  short Y() const;
  short Height() const;
  unsigned short Angle() const;

  // mutators
  void SetX(short x);
  void SetY(short y);
  void SetHeight(short height);
  void SetAngle(unsigned short angle);

  // byteorder conversion
  void ConvertToNetwork();
  void ConvertToHost();

  // read from string, write to string
  int Parse(const TCHAR* str);
  void UnParse(TCHAR* str, int strlen) const;

  int DistanceXY2(const LmPosition& pos) const;

  void Dump(FILE* f, int indent = 0) const;
  void Dump1(FILE* f) const;

private:

  lyra_position_t pos_;

};

#ifdef USE_INLINE
#include "LmPosition.i"
#endif

// inline functions

inline int operator==(const LmPosition& p1, const LmPosition& p2)
{
  // only x, y, z count
  return (p1.X() == p2.X()) && (p1.Y() == p2.Y()) && (p1.Height() == p2.Height());
}

#endif /* INCLUDED_LmPosition */
