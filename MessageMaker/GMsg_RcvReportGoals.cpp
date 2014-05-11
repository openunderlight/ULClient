// GMsg_RcvReportGoals.cpp  -*- C++ -*-
// $Id: GMsg_RcvReportGoals.cpp,v 1.6 1998-02-03 23:33:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_RcvReportGoals.h"
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

#include "GMsg_RcvReportGoals.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_RcvReportGoals.i"
#endif

////
// constructor
////

GMsg_RcvReportGoals::GMsg_RcvReportGoals()
  : LmMesg(GMsg::RCVREPORTGOALS, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN);
}

////
// destructor
////

GMsg_RcvReportGoals::~GMsg_RcvReportGoals()
{
  // empty
}

////
// Init
////

void GMsg_RcvReportGoals::Init(lyra_id_t goalid)
{
  SetGoalID(goalid);
}

////
// hton
////

void GMsg_RcvReportGoals::hton()
{
  HTONL(data_.goalid);
  // not converted: Summary
}

////
// ntoh
////

void GMsg_RcvReportGoals::ntoh()
{
  NTOHL(data_.goalid);
  // not converted: Summary
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_RcvReportGoals::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_RcvReportGoals[%p,%d]: "), this, sizeof(GMsg_RcvReportGoals));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("goal=%u >\n"), GoalID());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */


