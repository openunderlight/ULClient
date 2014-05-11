// GMsg_ItemDescription.cpp  -*- C++ -*-
// $Id: GMsg_ItemDescription.cpp,v 1.1 1997-12-05 14:44:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_ItemDescription.h"
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

#include "GMsg_ItemDescription.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_ItemDescription.i"
#endif

////
// constructor
////

GMsg_ItemDescription::GMsg_ItemDescription()
  : LmMesg(GMsg::ITEMDESCRIPTION, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0, _T("nobody"), _T("none"), _T("none"));
}

////
// destructor
////

GMsg_ItemDescription::~GMsg_ItemDescription()
{
  // empty
}

////
// Init
////

void GMsg_ItemDescription::Init(int serial, const TCHAR* creator, const TCHAR* description, const TCHAR* target)
{
  SetSerial(serial);
  SetCreator(creator);
  SetDescription(description);
  SetTarget(target);
}

////
// hton
////

void GMsg_ItemDescription::hton()
{
  HTONL(data_.serial);
  // not converted: Creator
  // not converted: Description
  // not converted: Target
}

////
// ntoh
////

void GMsg_ItemDescription::ntoh()
{
  NTOHL(data_.serial);
  // not converted: Creator
  // not converted: Description
  // not converted: Target
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_ItemDescription::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_ItemDescription[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("serial=%d creator='%s' description='%s'>\n"));
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetCreator
////

void GMsg_ItemDescription::SetCreator(const TCHAR* creator)
{
 _tcsnccpy(data_.creator, creator, sizeof(data_.creator));
  TRUNC(data_.creator, sizeof(data_.creator));
}

////
// SetDescription
////

void GMsg_ItemDescription::SetDescription(const TCHAR* description)
{
 _tcsnccpy(data_.description, description, sizeof(data_.description));
  TRUNC(data_.description, sizeof(data_.description));
  calc_size();
}

////
// SetTarget
////

void GMsg_ItemDescription::SetTarget(const TCHAR* target)
{
 _tcsnccpy(data_.target, target, sizeof(data_.target));
  TRUNC(data_.target, sizeof(data_.target));
}


////
// calc_size
////

void GMsg_ItemDescription::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.description);
  // add string length, plus 1 for null
  size += (_tcslen(data_.description) + 1);
  SetMessageSize(size);
}
