// GMsg_Error.i  -*- C++ -*-
// $Id: GMsg_Error.i,v 1.5 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_Error::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_Error::MsgType() const
{
  return data_.msg_type;
}

INLINE int GMsg_Error::ErrorStringLength() const
{
  return data_.err_len;
}

INLINE const TCHAR* GMsg_Error::ErrorString() const
{
  return data_.err_msg;
}

INLINE void GMsg_Error::SetMsgType(int mesgtype)
{
  data_.msg_type = mesgtype;
}
