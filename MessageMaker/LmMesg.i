// LmMesg.i  -*- C++ -*-
// $Id: LmMesg.i,v 1.3 1997-07-18 16:04:59-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void LmMesg::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE const void* LmMesg::MessageAddress() const
{
  return dptr_;
}

INLINE int LmMesg::ByteOrder() const
{
  return byteorder_;
}

INLINE int LmMesg::MessageSize() const
{
  return msgsize_;
}

INLINE int LmMesg::MaxMessageSize() const
{
  return max_msgsize_;
}

INLINE int LmMesg::MessageType() const
{
  return msgtype_;
}

INLINE void LmMesg::SetMessageAddress(void* dptr)
{
  dptr_ = dptr;
}

INLINE void LmMesg::SetByteOrder(int byteorder)
{
  byteorder_ = byteorder;
}

INLINE void LmMesg::SetMessageSize(int msgsize)
{
  msgsize_ = msgsize;
}

INLINE void LmMesg::SetMaxMessageSize(int max_msgsize)
{
  max_msgsize_ = max_msgsize;
}

INLINE void LmMesg::SetMessageType(int msgtype)
{
  msgtype_ = msgtype;
}
