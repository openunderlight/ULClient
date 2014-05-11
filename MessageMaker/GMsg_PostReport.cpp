// GMsg_PostReport.cpp  -*- C++ -*-
// $Id: GMsg_PostReport.cpp,v 1.5 1997-08-06 00:41:49-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_PostReport.h"
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

#include "GMsg_PostReport.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_PostReport.i"
#endif

////
// constructor
////

GMsg_PostReport::GMsg_PostReport()
  : LmMesg(GMsg::POSTREPORT, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, 0, _T("Recipient"), _T("Summary"), _T("Text"));
}

////
// destructor
////

GMsg_PostReport::~GMsg_PostReport()
{
  // empty
}

////
// Init
////

void GMsg_PostReport::Init(lyra_id_t goalid, int awardxp, const TCHAR* recipient, const TCHAR* summary, const TCHAR* reporttext)
{
  SetGoalID(goalid);
  SetAwardXP(awardxp);
  SetRecipient(recipient);
  SetSummary(summary);
  SetReportText(reporttext);
}

////
// hton
////

void GMsg_PostReport::hton()
{
  HTONL(data_.goalid);
  HTONL(data_.awardxp);
  // not converted: Recipient
  // not converted: Summary
  // not converted: ReportText
}

////
// ntoh
////

void GMsg_PostReport::ntoh()
{
  NTOHL(data_.goalid);
  NTOHL(data_.awardxp);
  // not converted: Recipient
  // not converted: Summary
  // not converted: ReportText
  calc_size(); // variable-size message
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_PostReport::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_PostReport[%p,%d]: "), this, sizeof(GMsg_PostReport));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("goalid=%u xp=%d recipient='%s' summary='%s' text='%s'>\n"),
	    GoalID(), AwardXP(), Recipient(), Summary(), ReportText());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetRecipient
////

void GMsg_PostReport::SetRecipient(const TCHAR* recipient)
{
 _tcsnccpy(data_.recipient, recipient, sizeof(data_.recipient));
  TRUNC(data_.recipient, sizeof(data_.recipient));
}

////
// SetSummary
////

void GMsg_PostReport::SetSummary(const TCHAR* summary)
{
 _tcsnccpy(data_.summary, summary, sizeof(data_.summary));
  TRUNC(data_.summary, sizeof(data_.summary));
}

////
// SetReportText
////

void GMsg_PostReport::SetReportText(const TCHAR* reporttext)
{
 _tcsnccpy(data_.reporttext, reporttext, sizeof(data_.reporttext));
  TRUNC(data_.reporttext, sizeof(data_.reporttext));
  calc_size();
}

////
// calc_size
////

void GMsg_PostReport::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.reporttext);
  // add string length, plus 1 for null
  size += (_tcslen(data_.reporttext) + 1);
  SetMessageSize(size);
}
