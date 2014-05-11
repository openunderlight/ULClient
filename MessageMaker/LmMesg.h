// LmMesg.h  -*- C++ -*-
// $Id: LmMesg.h,v 1.18 1997-07-21 16:41:56-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// Lyra Message abstract base class

#ifndef INCLUDED_LmMesg
#define INCLUDED_LmMesg

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"

// forward declarations

class LmMesgBuf;

// message class

class LmMesg {

public:

  LmMesg(int msgtype, int max_msgsize, int msgsize, void* dptr);
  virtual ~LmMesg();

  // initialize from incoming message buffer
  int Read(LmMesgBuf& ibuf);
  int Read(LmMesgBuf& ibuf, unsigned int num_bytes);

  // byteorder conversion
  void ConvertToNetwork();
  void ConvertToHost();

  // selectors
  int ByteOrder() const;
  int MessageSize() const;
  int MaxMessageSize() const;
  int MessageType() const;

  // return address of message data for copying out
  const void* MessageAddress() const;

  virtual void Dump(FILE* f, int indent = 0) const;
  void SetMessageType(int msgtype);

protected:

  // perform byteorder conversions: must be implemented in derived class
  virtual void hton() = 0;
  virtual void ntoh() = 0;

  // initialize from "raw bits" in network order
  int InitRaw(const void* data, int datalen);

  // mutators
  void SetMessageAddress(void* dptr);
  void SetByteOrder(int byteorder);
  void SetMessageSize(int msgsize);
  void SetMaxMessageSize(int max_msgsize);

private:

  // operations/methods not implemented
  LmMesg(const LmMesg&);
  //operator=(const LmMesg&);

  short byteorder_;    // message byte order
  short msgtype_;      // message type
  short max_msgsize_;  // maximum message size
  short msgsize_;      // message size

  void* dptr_;         // pointer to message data; must point to max_msgsize_ bytes

};

#ifdef USE_INLINE
#include "LmMesg.i"
#endif

#endif /* INCLUDED_LmMesg */
