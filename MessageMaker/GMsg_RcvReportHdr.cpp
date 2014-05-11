// GMsg_RcvReportHdr.cpp  -*- C++ -*-
// $Id: GMsg_RcvReportHdr.cpp,v 1.6 1998-02-03 23:33:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_RcvReportHdr.h"
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

#include "GMsg_RcvReportHdr.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_RcvReportHdr.i"
#endif

////
// constructor
////

GMsg_RcvReportHdr::GMsg_RcvReportHdr()
  : LmMesg(GMsg::RCVREPORTHDR, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, Lyra::ID_UNKNOWN, 0, _T("Summary"), 0);
}

////
// destructor
////

GMsg_RcvReportHdr::~GMsg_RcvReportHdr()
{
  // empty
}

////
// Init
////

void GMsg_RcvReportHdr::Init(lyra_id_t reportid, lyra_id_t goalid, short sessionid, const TCHAR* summary, unsigned short flags)
{
  SetReportID(reportid);
  SetGoalID(goalid);
  SetSessionID(sessionid);
  SetFlags(flags);
  SetSummary(summary);
}

////
// hton
////

void GMsg_RcvReportHdr::hton()
{
  HTONL(data_.reportid);
  HTONL(data_.goalid);
  HTONS(data_.sessionid);
  HTONS(data_.flags);
  // not converted: Summary
}

////
// ntoh
////

void GMsg_RcvReportHdr::ntoh()
{
  NTOHL(data_.reportid);
  NTOHL(data_.goalid);
  NTOHS(data_.flags);
  NTOHS(data_.sessionid);
  // not converted: Summary
  calc_size(); // variable-size message
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_RcvReportHdr::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_RcvReportHdr[%p,%d]: "), this, sizeof(GMsg_RcvReportHdr));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("id=%u goal=%u sessionid=%u flags=%u summary='%s'>\n"), ReportID(), GoalID(), SessionID(), Flags(), Summary());
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

void GMsg_RcvReportHdr::SetSummary(const TCHAR* summary)
{
 _tcsnccpy(data_.summary, summary, sizeof(data_.summary));
  TRUNC(data_.summary, sizeof(data_.summary));
  calc_size();
}

////
// calc_size
////

void GMsg_RcvReportHdr::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.summary);
  // add string length, plus 1 for null
  size += (_tcslen(data_.summary) + 1);
  SetMessageSize(size);
}
