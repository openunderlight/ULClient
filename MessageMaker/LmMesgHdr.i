// LmMesgHdr.i  -*- C++ -*-
// $Id: LmMesgHdr.i,v 1.3 1997-07-18 16:04:59-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void LmMesgHdr::Dump(FILE*, int) const
{
  // empty
}

INLINE void LmMesgHdr::Dump1(FILE*) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int LmMesgHdr::ByteOrder() const
{
  return byteorder_;
}

INLINE int LmMesgHdr::HeaderSize()
{
  return sizeof(lyra_mesg_hdr_t);
}

INLINE void* LmMesgHdr::HeaderAddress()
{
  return &hdr_;
}

INLINE const void* LmMesgHdr::HeaderAddress() const
{
  return &hdr_;
}

INLINE void LmMesgHdr::SetByteOrder(int byteorder)
{
  byteorder_ = byteorder;
}

INLINE void LmMesgHdr::SetMessageType(int mtype)
{
  hdr_.mesg_type = mtype;
}

INLINE void LmMesgHdr::SetMessageSize(int msize)
{
  hdr_.mesg_size = msize;
}
