// GMsg_RcvGoalbookHdr.cpp  -*- C++ -*-
// $Id: GMsg_RcvGoalbookHdr.cpp,v 1.1 1997-08-18 22:00:58-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_RcvGoalbookHdr.h"
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

#include "GMsg_RcvGoalbookHdr.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_RcvGoalbookHdr.i"
#endif

////
// constructor
////

GMsg_RcvGoalbookHdr::GMsg_RcvGoalbookHdr()
  : LmMesg(GMsg::RCVGOALBOOKHDR, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, -1, 0, _T("Summary"));
}

////
// destructor
////

GMsg_RcvGoalbookHdr::~GMsg_RcvGoalbookHdr()
{
  // empty
}

////
// Init
////

void GMsg_RcvGoalbookHdr::Init(lyra_id_t goalid, short guild, short rank, const TCHAR* summary)
{
  SetGoalID(goalid);
  SetGuild(guild);
  SetRank(rank);
  SetSummary(summary);
}

////
// hton
////

void GMsg_RcvGoalbookHdr::hton()
{
  HTONL(data_.goalid);
  HTONS(data_.guild);
  HTONS(data_.rank);
  // not converted: Summary
}

////
// ntoh
////

void GMsg_RcvGoalbookHdr::ntoh()
{
  NTOHL(data_.goalid);
  NTOHS(data_.guild);
  NTOHS(data_.rank);
  // not converted: Summary
  calc_size(); // variable-size message
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_RcvGoalbookHdr::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_RcvGoalbookHdr[%p,%d]: "), this, sizeof(GMsg_RcvGoalbookHdr));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("id=%u guild=%u rank=%u summary='%s'>\n"), GoalID(), Guild(), Rank(), Summary());
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

void GMsg_RcvGoalbookHdr::SetSummary(const TCHAR* summary)
{
 _tcsnccpy(data_.summary, summary, sizeof(data_.summary));
  TRUNC(data_.summary, sizeof(data_.summary));
  calc_size();
}

////
// calc_size
////

void GMsg_RcvGoalbookHdr::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.summary);
  // add string length, plus 1 for null
  size += (_tcslen(data_.summary) + 1);
  SetMessageSize(size);
}
