// GMsg_ItemDescription.i  -*- C++ -*-
// $Id: GMsg_ItemDescription.i,v 1.1 1997-12-05 14:44:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_ItemDescription::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_ItemDescription::Serial() const
{
  return data_.serial;
}

INLINE const TCHAR* GMsg_ItemDescription::Creator() const
{
  return data_.creator;
}

INLINE const TCHAR* GMsg_ItemDescription::Description() const
{
  return data_.description;
}

INLINE const TCHAR* GMsg_ItemDescription::Target() const
{
  return data_.target;
}


INLINE void GMsg_ItemDescription::SetSerial(int serial)
{
  data_.serial = serial;
}
