// LmMesgBuf.h  -*- C++ -*-
// $Id: LmMesgBuf.h,v 1.19 1997-09-01 14:59:59-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message buffer class

#ifndef INCLUDED_LmMesgBuf
#define INCLUDED_LmMesgBuf

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LmMesgHdr.h"

// forward declarations

class LmMesg;

// message buffer

class LmMesgBuf {

public:

  // constants
  enum {
    MIN_MSG_SIZE = 32,
    DEFAULT_MSG_SIZE = 64,
    MAX_BUF_SIZE = Lyra::MSG_MAX_SIZE + 4 // message + header
  };

public:

  LmMesgBuf(int msize = DEFAULT_MSG_SIZE);
  virtual ~LmMesgBuf();

  // initialize from a message header, or from a message itself
  void ReadMessage(LmMesg& msg);  // cannot be const
  void ReadHeader(const LmMesgHdr& hdr);

  // selectors for buffer sections
  int MessageSize(int delta = 0) const;
  void* MessageAddress(int offset = 0);
  const void* MessageAddress() const;
  int HeaderSize(int delta = 0) const;
  void* HeaderAddress(int offset = 0);
  const void* HeaderAddress() const;

  // selectors for entire buffer
  int BufferSize(int delta = 0) const;
  void* BufferAddress(int offset = 0);
  const void* BufferAddress() const;
  int MaxBufferSize() const;
  int MaxMessageSize() const;

  const LmMesgHdr& Header() const;

  void Dump(FILE* f, int indent = 0) const;

protected:

  // memory management methods
  void Alloc(int bufsize);
  void Free();

private:

  // operations/methods not implemented
  LmMesgBuf(const LmMesgBuf&);
  //operator=(const LmMesgBuf&);

  // message header
  LmMesgHdr hdr_;

  // buffer pointer: buffer stores header and message bits, in network order
  char* buf_;

  // current and maximum size
  int bufsize_;
  int max_bufsize_;
};

#ifdef USE_INLINE
#include "LmMesgBuf.i"
#endif

#endif /* INCLUDED_LmMesgBuf */
