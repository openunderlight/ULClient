// LmMesgBuf.i  -*- C++ -*-
// $Id: LmMesgBuf.i,v 1.5 1997-07-18 16:04:59-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void LmMesgBuf::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int LmMesgBuf::BufferSize(int delta) const
{
  return bufsize_ - delta;
}

INLINE void* LmMesgBuf::BufferAddress(int offset)
{
  return buf_ + offset;
}

INLINE const void* LmMesgBuf::BufferAddress() const
{
  return buf_;
}

INLINE int LmMesgBuf::MaxBufferSize() const
{
  return max_bufsize_;
}

INLINE int LmMesgBuf::MaxMessageSize() const
{
  return max_bufsize_ - hdr_.HeaderSize();
}

INLINE const LmMesgHdr& LmMesgBuf::Header() const
{
  return hdr_;
}

INLINE int LmMesgBuf::MessageSize(int delta) const
{
  return bufsize_ - hdr_.HeaderSize() - delta;
}

INLINE void* LmMesgBuf::MessageAddress(int offset)
{
  return buf_ + hdr_.HeaderSize() + offset;
}

INLINE const void* LmMesgBuf::MessageAddress() const
{
  return buf_ + hdr_.HeaderSize();
}

INLINE int LmMesgBuf::HeaderSize(int delta) const
{
  return hdr_.HeaderSize() - delta;
}

INLINE void* LmMesgBuf::HeaderAddress(int offset)
{
  return buf_ + offset;
}

INLINE const void* LmMesgBuf::HeaderAddress() const
{
  return buf_;
}
