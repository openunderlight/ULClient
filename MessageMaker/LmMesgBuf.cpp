// LmMesgBuf.cpp  -*- C++ -*-
// $Id: LmMesgBuf.cpp,v 1.20 1997-09-02 19:02:34-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// Implementation of LmMesgBuf class

#ifdef __GNUC__
#pragma implementation "LmMesgBuf.h"
#endif

#include <stdio.h>
#include <string.h>

#include "LmMesgBuf.h"
#include "LyraDefs.h"
#include "LmMesg.h"
#include "LmMesgHdr.h"

#ifndef USE_INLINE
#include "LmMesgBuf.i"
#endif

////
// constructor
////

LmMesgBuf::LmMesgBuf(int msize)
  : buf_(0),
    bufsize_(0),
    max_bufsize_(0)
{
  // check msize versus minimum
  if (msize < MIN_MSG_SIZE) {
    msize = MIN_MSG_SIZE;
  }
  // allocate buffer large enough for message of given size
  Alloc(msize + hdr_.HeaderSize());
}

////
// destructor
////

LmMesgBuf::~LmMesgBuf()
{
  Free();
}

////
// ReadMessage: initialize from an outgoing message body; will leave the message in
//   host byte order following return
////

void LmMesgBuf::ReadMessage(LmMesg& msg)
{
  // initialize header from message
  hdr_.Init(msg.MessageType(), msg.MessageSize());
  // figure out size of buffer and allocate it
  int bufsize = hdr_.HeaderSize() + hdr_.MessageSize();
  Alloc(bufsize);
  // copy header bits (in network order) into buffer
  hdr_.ConvertToNetwork();
  memcpy(HeaderAddress(), hdr_.HeaderAddress(), hdr_.HeaderSize());
  // copy message bits (in network order) into buffer
  msg.ConvertToNetwork();
  memcpy(MessageAddress(), msg.MessageAddress(), hdr_.MessageSize());
  msg.ConvertToHost();
}

////
// ReadHeader: initialize from an incoming message header
////

void LmMesgBuf::ReadHeader(const LmMesgHdr& hdr)
{
  // copy header fields into local header object
  hdr_.Init(hdr.MessageType(), hdr.MessageSize());
  // figure out size of buffer and allocate it
  int bufsize = hdr_.HeaderSize() + hdr_.MessageSize();
  Alloc(bufsize);
  // copy header bits (in network order) into buffer
  hdr_.ConvertToNetwork();
  memcpy(HeaderAddress(), hdr_.HeaderAddress(), hdr_.HeaderSize());
}

////
// Alloc: allocate buffer; if buffer is already allocated, and the
//   max size is large enough, then do not reallocate memory.
//   in either case, zeroes out buffer area.
////

void LmMesgBuf::Alloc(int bufsize)
{
  // check versus maximum buffer size
  if (bufsize > MAX_BUF_SIZE) {
    bufsize = MAX_BUF_SIZE;
  }
  // do we need to re-allocate?
  if (bufsize > max_bufsize_) {
    Free();
    if (bufsize > 0) {
      buf_ = LmNEW(TCHAR[bufsize]);
      max_bufsize_ = bufsize;
    }
  }
  // always set the "current" size to be whatever was requested,
  // even if buffer was not re-allocated
  bufsize_ = bufsize;
  // zero out buffer for good measure
  memset(buf_, 0, bufsize_);
}

////
// Free: deallocate buffer
////

void LmMesgBuf::Free()
{
  // has buffer been allocated yet?
  if (buf_) {
    LmDELETEARRAY(buf_);
    bufsize_ = 0;
    max_bufsize_ = 0;
  }
}

////
// Dump: print to given FILE stream
////

#ifdef USE_DEBUG
void LmMesgBuf::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmMesgBuf[%p,%d]: baddr=[%p] bsize=%d/%d msize=%d/%d>\n"),
	  this, sizeof(LmMesgBuf),
	  BufferAddress(), BufferSize(), MaxBufferSize(), MessageSize(), MaxMessageSize());
  // print associated message header object
  hdr_.Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
