// RMsg_Error.cpp  -*- C++ -*-
// $Id: RMsg_Error.cpp,v 1.9 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_Error.h"
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

#include "RMsg_Error.h"
#include "LyraDefs.h"
#include "RMsg.h"

#ifndef USE_INLINE
#include "RMsg_Error.i"
#endif

////
// constructor
////

RMsg_Error::RMsg_Error()
  : LmMesg(RMsg::SERVERERROR, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::MSG_NONE, _T("error"));
}

////
// destructor
////

RMsg_Error::~RMsg_Error()
{
  // empty
}

////
// Init
////

void RMsg_Error::Init(int mesgtype, const TCHAR* errmsg)
{
  SetMsgType(mesgtype);
  SetErrorString(errmsg);
}

////
// hton
////

void RMsg_Error::hton()
{
  HTONS(data_.msg_type);
  HTONS(data_.err_len);
  // no conversion: data_.err_msg
}

////
// ntoh
////

void RMsg_Error::ntoh()
{
  NTOHS(data_.msg_type);
  NTOHS(data_.err_len);
  // no conversion: data_.err_msg
  calc_size(); // variable-size message
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_Error::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_Error[%p,%d]: "), this, sizeof(RMsg_Error));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("type=%d err='%s'>\n"), MsgType(), ErrorString());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetErrorString
////

void RMsg_Error::SetErrorString(const TCHAR* errstr)
{
 _tcsnccpy(data_.err_msg, errstr, sizeof(data_.err_msg));
  TRUNC(data_.err_msg, sizeof(data_.err_msg));
  data_.err_len =_tcslen(data_.err_msg);
  calc_size();
}

////
// calc_size
////

void RMsg_Error::calc_size()
{
  // initial size: entire structure, minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.err_msg);
  // add length of error message, plus a trailing null
  size += ErrorStringLength() + 1;
  SetMessageSize(size);
}
