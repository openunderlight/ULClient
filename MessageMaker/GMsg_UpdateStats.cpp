// GMsg_UpdateStats.cpp  -*- C++ -*-
// $Id: GMsg_UpdateStats.cpp,v 1.8 1997-07-29 19:32:10-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_UpdateStats.h"
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

#include "GMsg_UpdateStats.h"

#ifndef USE_INLINE
#include "GMsg_UpdateStats.i"
#endif

////
// constructor
////

GMsg_UpdateStats::GMsg_UpdateStats()
  : LmMesg(GMsg::UPDATESTATS, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  //Init(LmStats::DEFAULT_INSTANCE, LmArts::DEFAULT_INSTANCE);
}

////
// destructor
////

GMsg_UpdateStats::~GMsg_UpdateStats()
{
  // empty
}

////
// Init
////

void GMsg_UpdateStats::Init(const LmStats& stats, const LmArts& arts)
{
  SetPlayerStats(stats);
  SetArts(arts);
}

////
// hton
////

void GMsg_UpdateStats::hton()
{
  data_.stats.ConvertToNetwork();
  data_.arts.ConvertToNetwork();
}

////
// ntoh
////

void GMsg_UpdateStats::ntoh()
{
  data_.stats.ConvertToHost();
  data_.arts.ConvertToHost();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_UpdateStats::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_UpdateStats[%p,%d]: "), this, sizeof(GMsg_UpdateStats));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T(">\n"));
    PlayerStats().Dump(f, indent + 1);
    Arts().Dump(f, indent + 1);
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
