// GMsg_UsePPoint.cpp  -*- C++ -*-
// $Id: GMsg_UsePPoint.cpp,v 1.4 1997-12-05 14:44:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_UsePPoint.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock2.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */
#include <stdio.h>
#include <string.h>

#include "GMsg_UsePPoint.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_UsePPoint.i"
#endif

////
// constructor
////

GMsg_UsePPoint::GMsg_UsePPoint()
  : LmMesg(GMsg::USEPPOINT, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0, 0, 0, 0);
}

////
// destructor
////

GMsg_UsePPoint::~GMsg_UsePPoint()
{
  // empty
}

////
// Init
////

void GMsg_UsePPoint::Init(short how, short var1, short var2, short var3)
{
  SetHow(how);
  SetVar1(var1);
  SetVar2(var2);
  SetVar3(var3);
}

////
// hton
////

void GMsg_UsePPoint::hton()
{
  HTONS(data_.how);
  HTONS(data_.var1);
  HTONS(data_.var2);
  HTONS(data_.var3);
}

////
// ntoh
////

void GMsg_UsePPoint::ntoh()
{
  NTOHS(data_.how);
  NTOHS(data_.var1);
  NTOHS(data_.var2);
  NTOHS(data_.var3);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_UsePPoint::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_UsePPoint[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("how=%d var1=%d var2=%d var3=%d>\n"), How(), Var1(), Var2(), Var3());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

