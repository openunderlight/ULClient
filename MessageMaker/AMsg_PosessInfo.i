// AMsg_PosessInfo.i  -*- C++ -*-
// $Id: AMsg_PosessInfo.i,v 1.1 1997-06-18 15:39:42-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void AMsg_PosessInfo::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int AMsg_PosessInfo::Status() 
{
  return data_.status;
}

INLINE TCHAR* AMsg_PosessInfo::Name() 
{
  return data_.username;
}

INLINE TCHAR* AMsg_PosessInfo::Password() 
{
  return data_.password;
}

INLINE void AMsg_PosessInfo::SetStatus(int value) 
{
  data_.status = value;
}

INLINE void AMsg_PosessInfo::SetName(TCHAR *value)
{
  _tcscpy(data_.username, value);
}

INLINE void AMsg_PosessInfo::SetPassword(TCHAR *value)
{
  _tcscpy(data_.password, value);
}