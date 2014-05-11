// LmMesgHdr.h  -*- C++ -*-
// $Id: LmMesgHdr.h,v 1.13 1997-07-09 19:24:52-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// Message Header class

#ifndef INCLUDED_LmMesgHdr
#define INCLUDED_LmMesgHdr

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"

// forward references

// base datatypes

typedef unsigned short lyra_mesg_size_t;
typedef unsigned short lyra_mesg_type_t;

struct lyra_mesg_hdr_t {
  lyra_mesg_type_t mesg_type;
  lyra_mesg_size_t mesg_size;
};

// message header

class LmMesgHdr {

public:

  // constants
  enum {
    DEFAULT_MSGTYPE = 0,
    DEFAULT_MSGSIZE = 0
  };

public:

  LmMesgHdr();

  // re-constructor
  void Init(int mtype, int msize);

  // conversion functions
  void ConvertToNetwork();
  void ConvertToHost();

  // message header fields (always return values in host byteorder)
  int MessageType() const;
  int MessageSize() const;

  // selectors
  int ByteOrder() const;
  static int HeaderSize();
  void* HeaderAddress();
  const void* HeaderAddress() const;

  // mutators
  void SetMessageType(int mtype);
  void SetMessageSize(int msize);
  void SetByteOrder(int byteorder);

  void Dump(FILE* f, int indent = 0) const;
  void Dump1(FILE* f) const;

private:

  // header data
  lyra_mesg_hdr_t hdr_;

  // byte order of hdr_
  int byteorder_;

};

#ifdef USE_INLINE
#include "LmMesgHdr.i"
#endif

#endif /* INCLUDED_LmMesgHdr */
