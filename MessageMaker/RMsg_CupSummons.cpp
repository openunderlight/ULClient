// RMsg_CupSummons.cpp  -*- C++ -*-
// $Id: RMsg_CupSummons.cpp,v 1.1 1997-11-25 15:53:47-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_CupSummons.h"
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

#include "RMsg_CupSummons.h"
#include "LyraDefs.h"
#include "RMsg.h"

#ifndef USE_INLINE
#include "RMsg_CupSummons.i"
#endif

////
// constructor
////

RMsg_CupSummons::RMsg_CupSummons()
  : LmMesg(RMsg::CUPSUMMONS, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init();
}

////
// destructor
////

RMsg_CupSummons::~RMsg_CupSummons()
{
  // empty
}

////
// Init
////

void RMsg_CupSummons::Init(void)
{
	// empty
}

////
// hton
////

void RMsg_CupSummons::hton()
{
    // not converted: PlayerName
}

////
// ntoh
////

void RMsg_CupSummons::ntoh()
{
    // not converted: PlayerName
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_CupSummons::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_CupSummons[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
     _ftprintf(f, _T("player='%s'\n"), PlayerName());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetPlayerName
////

void RMsg_CupSummons::SetPlayerName(const TCHAR* playername)
{
 _tcsnccpy(data_.playername, playername, sizeof(data_.playername));
  TRUNC(data_.playername, sizeof(data_.playername));
}
