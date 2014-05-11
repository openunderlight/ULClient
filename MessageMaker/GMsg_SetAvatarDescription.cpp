// GMsg_SetAvatarDescription.cpp  -*- C++ -*-
// $Id: GMsg_SetAvatarDescription.cpp,v 1.1 1998-04-17 16:25:53-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_SetAvatarDescription.h"
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

#include "GMsg_SetAvatarDescription.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_SetAvatarDescription.i"
#endif

////
// constructor
////

GMsg_SetAvatarDescription::GMsg_SetAvatarDescription()
  : LmMesg(GMsg::SETAVATARDESCRIPTION, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(_T("nothing"));
}

////
// destructor
////

GMsg_SetAvatarDescription::~GMsg_SetAvatarDescription()
{
  // empty
}

////
// Init
////

void GMsg_SetAvatarDescription::Init(const TCHAR* description)
{
  SetDescription(description);
}

////
// hton
////

void GMsg_SetAvatarDescription::hton()
{
  // not converted: Description
}

////
// ntoh
////

void GMsg_SetAvatarDescription::ntoh()
{
  // not converted: Description
  calc_size(); // variable-length message
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_SetAvatarDescription::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_SetAvatarDescription[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("desc='%s'>\n"), Description());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetDescription
////

void GMsg_SetAvatarDescription::SetDescription(const TCHAR* description)
{
 _tcsnccpy(data_.description, description, sizeof(data_.description));
  TRUNC(data_.description, sizeof(data_.description));
  calc_size();
}

////
// calc_size
////

void GMsg_SetAvatarDescription::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.description);
  // add string length, plus 1 for null
  size += (_tcslen(data_.description) + 1);
  SetMessageSize(size);
}
