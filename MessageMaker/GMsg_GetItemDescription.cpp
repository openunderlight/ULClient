// GMsg_GetItemDescription.cpp  -*- C++ -*-
// $Id: GMsg_GetItemDescription.cpp,v 1.2 1997-12-11 16:25:07-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_GetItemDescription.h"
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

#include "GMsg_GetItemDescription.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_GetItemDescription.i"
#endif

////
// constructor
////

GMsg_GetItemDescription::GMsg_GetItemDescription()
  : LmMesg(GMsg::GETITEMDESCRIPTION, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(LmItemHdr::DEFAULT_INSTANCE);
}

////
// destructor
////

GMsg_GetItemDescription::~GMsg_GetItemDescription()
{
  // empty
}

////
// Init
////

void GMsg_GetItemDescription::Init(const LmItemHdr& itemheader)
{
  SetItemHeader(itemheader);
}

////
// hton
////

void GMsg_GetItemDescription::hton()
{
  data_.itemheader.ConvertToNetwork();
}

////
// ntoh
////

void GMsg_GetItemDescription::ntoh()
{
  data_.itemheader.ConvertToHost();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_GetItemDescription::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_GetItemDescription[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("data>\n"));
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
