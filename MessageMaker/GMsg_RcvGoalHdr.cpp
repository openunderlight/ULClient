// GMsg_RcvGoalHdr.cpp  -*- C++ -*-
// $Id: GMsg_RcvGoalHdr.cpp,v 1.6 1998-02-03 23:33:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_RcvGoalHdr.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */
#include <stdio.h>
#include <string.h>

#include "GMsg_RcvGoalHdr.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_RcvGoalHdr.i"
#endif

////
// constructor
////

GMsg_RcvGoalHdr::GMsg_RcvGoalHdr()
  : LmMesg(GMsg::RCVGOALHDR, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, 0, 0, 0, _T("Summary"));
}

////
// destructor
////

GMsg_RcvGoalHdr::~GMsg_RcvGoalHdr()
{
  // empty
}

////
// Init
////

void GMsg_RcvGoalHdr::Init(lyra_id_t goalid, int sessionid, short status, short playeroption, const TCHAR* summary)
{
  SetGoalID(goalid);
  SetSessionID(sessionid);
  SetStatus(status);
  SetPlayerOption(playeroption);
  SetSummary(summary);
}

////
// hton
////

void GMsg_RcvGoalHdr::hton()
{
  HTONL(data_.goalid);
  HTONL(data_.sessionid);
  HTONS(data_.status);
  HTONS(data_.playeroption);
  // not converted: Summary
}

////
// ntoh
////

void GMsg_RcvGoalHdr::ntoh()
{
  NTOHL(data_.goalid);
  NTOHL(data_.sessionid);
  NTOHS(data_.status);
  NTOHS(data_.playeroption);
  // not converted: Summary
  calc_size(); // variable-size message
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_RcvGoalHdr::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_RcvGoalHdr[%p,%d]: "), this, sizeof(GMsg_RcvGoalHdr));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("id=%u sessionid=%u status=%d playeroption=%d summary='%s'>\n"), GoalID(), SessionID(), Status(), PlayerOption(), Summary());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetSummary
////

void GMsg_RcvGoalHdr::SetSummary(const TCHAR* summary)
{
 _tcsnccpy(data_.summary, summary, sizeof(data_.summary));
  TRUNC(data_.summary, sizeof(data_.summary));
  calc_size();
}

////
// calc_size
////

void GMsg_RcvGoalHdr::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.summary);
  // add string length, plus 1 for null
  size += (_tcslen(data_.summary) + 1);
  SetMessageSize(size);
}
