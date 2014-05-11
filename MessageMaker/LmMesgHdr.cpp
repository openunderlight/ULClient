// LmMesgHdr.cpp  -*- C++ -*-
// $Id: LmMesgHdr.cpp,v 1.13 1997-07-29 18:15:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// Implementation of LmMesgHdr class

#ifdef __GNUC__
#pragma implementation "LmMesgHdr.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock2.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */
#include <stdio.h>
#include <string.h>

#include "LmMesgHdr.h"
#include "LyraDefs.h"

#ifndef USE_INLINE
#include "LmMesgHdr.i"
#endif

////
// constructor
////

LmMesgHdr::LmMesgHdr()
{
  Init(DEFAULT_MSGTYPE, DEFAULT_MSGSIZE);
}

////
// Init: given type and length in host order
////

void LmMesgHdr::Init(int mtype, int msize)
{
  SetMessageType(mtype);
  SetMessageSize(msize);
  SetByteOrder(ByteOrder::HOST);
}

////
// ConvertToNetwork
////

void LmMesgHdr::ConvertToNetwork()
{
  if (byteorder_ != ByteOrder::NETWORK) {
    HTONS(hdr_.mesg_type);
    HTONS(hdr_.mesg_size);
    byteorder_ = ByteOrder::NETWORK;
  }
}

////
// ConvertToHost
////

void LmMesgHdr::ConvertToHost()
{
  if (byteorder_ != ByteOrder::HOST) {
    NTOHS(hdr_.mesg_type);
    NTOHS(hdr_.mesg_size);
    byteorder_ = ByteOrder::HOST;
  }
}

////
// MessageType: return in host order always
////

int LmMesgHdr::MessageType() const
{
  return (byteorder_ == ByteOrder::HOST) ? hdr_.mesg_type : ntohs(hdr_.mesg_type);
}

////
// MessageSize: return in host order always
////

int LmMesgHdr::MessageSize() const
{
  return (byteorder_ == ByteOrder::HOST) ? hdr_.mesg_size : ntohs(hdr_.mesg_size);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void LmMesgHdr::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmMesgHdr[%p,%d]: order=%c mtype=%d msize=%d hsize=%d haddr=%p>\n"), this, sizeof(LmMesgHdr),
	  ByteOrder(), MessageType(), MessageSize(), HeaderSize(), HeaderAddress());
}
#endif

////
// Dump1: print to FILE stream in "shorthand" format
////

#ifdef USE_DEBUG
void LmMesgHdr::Dump1(FILE* f) const
{
 _ftprintf(f, _T("[%d:%d]"), MessageType(), MessageSize());
}
#endif
