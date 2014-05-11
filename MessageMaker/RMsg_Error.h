// RMsg_Error.h  -*- C++ -*-
// $Id: RMsg_Error.h,v 1.10 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// generic server error message

#ifndef INCLUDED_RMsg_Error
#define INCLUDED_RMsg_Error

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"

// forward references

// message class

class RMsg_Error : public LmMesg {

public:

  enum {
    // constants
    ERRMSG_LEN = 80
  };

public:

  RMsg_Error();
  ~RMsg_Error();

  void Init(int mesgtype, const TCHAR* errmsg);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int MsgType() const;
  int ErrorStringLength() const;
  const TCHAR* ErrorString() const;

  // mutators
  void SetMsgType(int mesgtype);
  void SetErrorString(const TCHAR* errstr);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual message size
  void calc_size();

  // message data structure
  struct data_t {
    short msg_type;
    short err_len;
    TCHAR err_msg[ERRMSG_LEN];
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_Error.i"
#endif

#endif /* INCLUDED_RMsg_Error */
