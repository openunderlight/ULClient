// GMsg_UpdateStats.i  -*- C++ -*-
// $Id: GMsg_UpdateStats.i,v 1.6 1997-07-29 19:32:10-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_UpdateStats::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE const LmStats& GMsg_UpdateStats::PlayerStats() const
{
  return data_.stats;
}

INLINE const LmArts& GMsg_UpdateStats::Arts() const
{
  return data_.arts;
}

INLINE void GMsg_UpdateStats::SetPlayerStats(const LmStats& stats)
{
  data_.stats = stats;
}

INLINE void GMsg_UpdateStats::SetArts(const LmArts& arts)
{
  data_.arts = arts;
}
