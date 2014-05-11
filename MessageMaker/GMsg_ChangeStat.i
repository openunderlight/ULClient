// GMsg_ChangeStat.i  -*- C++ -*-
// $Id: GMsg_ChangeStat.i,v 1.4 1997-07-29 19:32:10-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_ChangeStat::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE bool GMsg_ChangeStat::change_num_ok(int change_num)
{
  return INDEX_OK(change_num, 0, MAX_CHANGES);
}

INLINE int GMsg_ChangeStat::NumChanges() const
{
  return num_changes_;
}

INLINE int GMsg_ChangeStat::RequestType(int change_num) const
{
  return change_num_ok(change_num) ? data_.changes[change_num].requesttype : 0;
}

INLINE int GMsg_ChangeStat::Stat(int change_num) const
{
  return change_num_ok(change_num) ? data_.changes[change_num].stat : 0;
}

INLINE int GMsg_ChangeStat::Value(int change_num) const
{
  return change_num_ok(change_num) ? data_.changes[change_num].value : 0;
}

INLINE void GMsg_ChangeStat::SetRequestType(int change_num, int requesttype)
{
  if (change_num_ok(change_num)) {
    data_.changes[change_num].requesttype = requesttype;
  }
}

INLINE void GMsg_ChangeStat::SetStat(int change_num, int stat)
{
  if (change_num_ok(change_num)) {
    data_.changes[change_num].stat = stat;
  }
}

INLINE void GMsg_ChangeStat::SetValue(int change_num, int value)
{
  if (change_num_ok(change_num)) {
    data_.changes[change_num].value = value;
  }
}

INLINE void GMsg_ChangeStat::InitChange(int change_num, int requesttype, int stat, int value)
{
  if (change_num_ok(change_num)) {
    data_.changes[change_num].requesttype = requesttype;
    data_.changes[change_num].stat = stat;
    data_.changes[change_num].value = value;
  }
}
