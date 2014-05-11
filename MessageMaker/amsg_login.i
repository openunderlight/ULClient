// AMsg_Login.i  -*- C++ -*-
// $Id: AMsg_Login.i,v 1.1 1997-06-17 15:34:01-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void AMsg_Login::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int AMsg_Login::Version() const
{
  return data_.version;
}

INLINE const TCHAR* AMsg_Login::PlayerName() const
{
  return data_.playername;
}

INLINE const TCHAR* AMsg_Login::Password() const
{
  return data_.password;
}

INLINE void AMsg_Login::SetVersion(int version)
{
  data_.version = version;
}
