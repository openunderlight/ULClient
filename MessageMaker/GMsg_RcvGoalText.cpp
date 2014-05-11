// GMsg_RcvGoalText.cpp  -*- C++ -*-
// $Id: GMsg_RcvGoalText.cpp,v 1.6 1997-08-18 18:00:57-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_RcvGoalText.h"
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

#include "GMsg_RcvGoalText.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_RcvGoalText.i"
#endif

////
// constructor
////

GMsg_RcvGoalText::GMsg_RcvGoalText()
  : LmMesg(GMsg::RCVGOALTEXT, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, -1, -1, _T("Creator"), _T("Text"));
  SetCreatorID(Lyra::ID_UNKNOWN);
}

////
// destructor
////

GMsg_RcvGoalText::~GMsg_RcvGoalText()
{
  // empty
}

////
// Init
////

void GMsg_RcvGoalText::Init(lyra_id_t goalid, short sugsphere, short sugstat, const TCHAR* creator,
			    const TCHAR* goaltext)
{
  SetGoalID(goalid);
  SetSugSphere(sugsphere);
  SetSugStat(sugstat);
  SetCreator(creator);
  SetGoalText(goaltext);
}

////
// hton
////

void GMsg_RcvGoalText::hton()
{
  HTONL(data_.goalid);
  HTONS(data_.sugsphere);
  HTONS(data_.sugstat);
  // not converted: Creator
  // not converted: GoalText
}

////
// ntoh
////

void GMsg_RcvGoalText::ntoh()
{
  NTOHL(data_.goalid);
  NTOHS(data_.sugsphere);
  NTOHS(data_.sugstat);
  // not converted: Creator
  // not converted: GoalText
  calc_size(); // variable-size message
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_RcvGoalText::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_RcvGoalText[%p,%d]: "), this, sizeof(GMsg_RcvGoalText));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("id=%u sugsphere=%d sugstat=%d creator(%u)='%s' text='%s'>\n"),
	    GoalID(), SugSphere(), SugStat(), CreatorID(), Creator(), GoalText());
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

void GMsg_RcvGoalText::SetCreator(const TCHAR* creator)
{
 _tcsnccpy(data_.creator, creator, sizeof(data_.creator));
  TRUNC(data_.creator, sizeof(data_.creator));
}

////
// SetGoalText
////

void GMsg_RcvGoalText::SetGoalText(const TCHAR* goaltext)
{
 _tcsnccpy(data_.goaltext, goaltext, sizeof(data_.goaltext));
  TRUNC(data_.goaltext, sizeof(data_.goaltext));
  calc_size();
}

////
// calc_size
////

void GMsg_RcvGoalText::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.goaltext);
  // add string length, plus 1 for null
  size += (_tcslen(data_.goaltext) + 1);
  SetMessageSize(size);
}
