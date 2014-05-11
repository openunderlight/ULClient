// LmPosition.cpp  -*- C++ -*-
// $Id: LmPosition.cpp,v 1.14 1997-07-29 18:15:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmPosition.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#endif
#include <stdio.h>
#include <string.h>

#include "LmPosition.h"
#include "LyraDefs.h"

#ifndef USE_INLINE
#include "LmPosition.i"
#endif

// default instance
const LmPosition LmPosition::DEFAULT_INSTANCE;

////
// constructor
////

LmPosition::LmPosition()
{
  Init(DEFAULT_X, DEFAULT_Y, DEFAULT_HEIGHT, DEFAULT_ANGLE);
}

////
// Init
////

void LmPosition::Init(short x, short y, short height, unsigned short angle)
{
  SetX(x);
  SetY(y);
  SetHeight(height);
  SetAngle(angle);
}

////
// ConvertToNetwork
////

void LmPosition::ConvertToNetwork()
{
  HTONS(pos_.x);
  HTONS(pos_.y);
  HTONS(pos_.height);
  HTONS(pos_.angle);
}

////
// ConvertToHost
////

void LmPosition::ConvertToHost()
{
  NTOHS(pos_.x);
  NTOHS(pos_.y);
  NTOHS(pos_.height);
  NTOHS(pos_.angle);
}

////
// Parse: read from string, return 0 if ok, -1 otherwise
//   format: "x y height angle"
////

int LmPosition::Parse(const TCHAR* str)
{
  int x, y, height, angle;
  if (_stscanf(str, _T("%d.%d.%d.%d"), &x, &y, &height, &angle) != 4) {
    //_tprintf(_T("LmPosition::Parse: %d %d %d %d\n"), x, y, height, angle);
    return -1;
  }
  Init(x, y, height, angle);
  return 0;
}

////
// UnParse: write to string
////
 
void LmPosition::UnParse(TCHAR* str, int /* strlen */) const
{
 _stprintf(str, _T("%d.%d.%d.%d"), X(), Y(), Height(), Angle());
}

////
// DistanceXY2: return X-Y (no Z) distance squared between position and given position
////

int LmPosition::DistanceXY2(const LmPosition& pos) const
{
  return (SQUARE(X() - pos.X()) + SQUARE(Y() - pos.Y()));
}

////
// Dump
////

#ifdef USE_DEBUG
void LmPosition::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmPosition[%p,%d]: x=%d y=%d height=%d angle=%d>\n"), this, sizeof(LmPosition),
	  X(), Y(), Height(), Angle());
}
#endif /* USE_DEBUG */

////
// Dump1
////

#ifdef USE_DEBUG
void LmPosition::Dump1(FILE* f) const
{
 _ftprintf(f, _T("[%d.%d.%d.%d]"), X(), Y(), Height(), Angle());
}
#endif /* USE_DEBUG */
