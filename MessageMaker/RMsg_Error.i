// RMsg_Error.i  -*- C++ -*-
// $Id: RMsg_Error.i,v 1.5 1997-07-18 17:26:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_Error::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int RMsg_Error::ErrorStringLength() const
{
  return data_.err_len;
}

INLINE int RMsg_Error::MsgType() const
{
  return data_.msg_type;
}

INLINE const TCHAR* RMsg_Error::ErrorString() const
{
  return data_.err_msg;
}

INLINE void RMsg_Error::SetMsgType(int mesgtype)
{
  data_.msg_type = mesgtype;
}
