// GMsg_RcvReportText.cpp  -*- C++ -*-
// $Id: GMsg_RcvReportText.cpp,v 1.7 1998-02-03 23:33:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_RcvReportText.h"
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

#include "GMsg_RcvReportText.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_RcvReportText.i"
#endif

////
// constructor
////

GMsg_RcvReportText::GMsg_RcvReportText()
  : LmMesg(GMsg::RCVREPORTTEXT, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, Lyra::ID_UNKNOWN, 0, 0, _T("Creator"), _T("Recipient"), _T("Text"));
  SetCreatorID(Lyra::ID_UNKNOWN);
  SetRecipientID(Lyra::ID_UNKNOWN);
}

////
// destructor
////

GMsg_RcvReportText::~GMsg_RcvReportText()
{
  // empty
}

////
// Init
////

void GMsg_RcvReportText::Init(lyra_id_t reportid, lyra_id_t goalid, int awardxp, int flags,
			      const TCHAR* creator, const TCHAR* recipient, const TCHAR* reporttext)
{
  SetReportID(reportid);
  SetGoalID(goalid);
  SetAwardXP(awardxp);
  SetFlags(flags);
  SetCreator(creator);
  SetRecipient(recipient);
  SetReportText(reporttext);
}

////
// hton
////

void GMsg_RcvReportText::hton()
{
  HTONL(data_.reportid);
  HTONL(data_.goalid);
  HTONL(data_.awardxp);
  HTONL(data_.flags);
  // not converted: Creator
  // not converted: Recipient
  // not converted: ReportText
}

////
// ntoh
////

void GMsg_RcvReportText::ntoh()
{
  NTOHL(data_.reportid);
  NTOHL(data_.goalid);
  NTOHL(data_.awardxp);
  NTOHL(data_.flags);
  // not converted: Creator
  // not converted: Recipient
  // not converted: ReportText
  calc_size(); // variable-size message
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_RcvReportText::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_RcvReportText[%p,%d]: "), this, sizeof(GMsg_RcvReportText));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("id=%u goal=%u, xp=%d, flags=%d, creator(%u)='%s' recipient(%u)='%s' text='%s'>\n"),
	    ReportID(), GoalID(), AwardXP(), Flags(), CreatorID(), Creator(), RecipientID(), Recipient(), ReportText());
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

void GMsg_RcvReportText::SetCreator(const TCHAR* creator)
{
 _tcsnccpy(data_.creator, creator, sizeof(data_.creator));
  TRUNC(data_.creator, sizeof(data_.creator));
}

////
// SetRecipient
////

void GMsg_RcvReportText::SetRecipient(const TCHAR* recipient)
{
 _tcsnccpy(data_.recipient, recipient, sizeof(data_.recipient));
  TRUNC(data_.recipient, sizeof(data_.recipient));
}

////
// SetReportText
////

void GMsg_RcvReportText::SetReportText(const TCHAR* reporttext)
{
 _tcsnccpy(data_.reporttext, reporttext, sizeof(data_.reporttext));
  TRUNC(data_.reporttext, sizeof(data_.reporttext));
  calc_size();
}

////
// calc_size
////

void GMsg_RcvReportText::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.reporttext);
  // add string length, plus 1 for null
  size += (_tcslen(data_.reporttext) + 1);
  SetMessageSize(size);
}
