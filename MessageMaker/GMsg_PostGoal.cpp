// GMsg_PostGoal.cpp  -*- C++ -*-
// $Id: GMsg_PostGoal.cpp,v 1.9 1997-11-17 14:04:49-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_PostGoal.h"
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

#include "GMsg_PostGoal.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_PostGoal.i"
#endif

////
// constructor
////

GMsg_PostGoal::GMsg_PostGoal()
  : LmMesg(GMsg::POSTGOAL, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, 0, 0, 0, 0, 0, 0, 0, _T("Summary"), _T("Text"), 0, 
	  0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Keywords"));
}

////
// destructor
////

GMsg_PostGoal::~GMsg_PostGoal()
{
  // empty
}

////
// Init
////

void GMsg_PostGoal::Init(lyra_id_t goalid, short level_num, short guild, 
			 short maxaccepted, short expirationtime, 
			 short sugsphere, short sugstat, short flags,
			 const TCHAR* summary, const TCHAR* goaltext, short voteexpire,
			 unsigned short graphic, unsigned char charges, unsigned char color1, unsigned char color2,
			 unsigned char item_type, unsigned int field1, unsigned int field2, 
			 unsigned int field3, unsigned int quest_xp, const TCHAR* keywords)
	
{
  SetGoalID(goalid);
  SetLevel(level_num);
  SetGuild(guild);
  SetMaxAccepted(maxaccepted);
  SetExpirationTime(expirationtime);
  SetSugSphere(sugsphere);
  SetSugStat(sugstat);
  SetSummary(summary);
  SetGoalText(goaltext);
  SetVoteExpiration(voteexpire);
  SetFlags(flags);
  SetKeywords(keywords);
  SetGraphic(graphic);
  SetCharges(charges);
  SetColor1(color1);
  SetColor2(color2);
  SetItemType(item_type);
  SetField1(field1);
  SetField2(field2);
  SetField3(field3);
  SetQuestXP(quest_xp);
}

////
// hton
////

void GMsg_PostGoal::hton()
{
  HTONL(data_.goalid);
  HTONS(data_.level_num);
  HTONS(data_.guild);
  HTONS(data_.maxaccepted);
  HTONS(data_.voteexpire);
  HTONS(data_.expirationtime);
  HTONS(data_.sugsphere);
  HTONS(data_.sugstat);
  HTONS(data_.flags);
  HTONS(data_.graphic);
  HTONL(data_.field1);
  HTONL(data_.field2);
  HTONL(data_.field3);
  HTONL(data_.quest_xp);

  // not converted: Summary
  // not converted: GoalText
  // not converted: Keywords
  // not converted: charges
  // not converted: color1
  // not converted: color2
  // not converted: item_type

}

////
// ntoh
////

void GMsg_PostGoal::ntoh()
{
  NTOHL(data_.goalid);
  NTOHS(data_.level_num);
  NTOHS(data_.guild);
  NTOHS(data_.maxaccepted);
  NTOHS(data_.voteexpire);
  NTOHS(data_.expirationtime);
  NTOHS(data_.sugsphere);
  NTOHS(data_.sugstat);
  NTOHS(data_.flags);
  NTOHS(data_.graphic);
  NTOHL(data_.field1);
  NTOHL(data_.field2);
  NTOHL(data_.field3);
  NTOHL(data_.quest_xp);

  // not converted: Summary
  // not converted: GoalText
  // not converted: Keywords
  // not converted: charges
  // not converted: color1
  // not converted: color2
  // not converted: item_type

  calc_size(); // variable-size message
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_PostGoal::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_PostGoal[%p,%d]: "), this, sizeof(GMsg_PostGoal));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("goalid=%u level=%d guild=%d maxaccept=%d expire=%d voteexpire=%d sphere=%d stat=%d flags=%d summary='%s' keywords='%s' graphic=%u charges=%c color1=%c color2=%c item_type=%c field1=%d field2=%d field3=%d quest_xp=%d text='%s'>\n"),
	    GoalID(), Level(), Guild(), MaxAccepted(), ExpirationTime(), VoteExpiration(),
	    SugSphere(), SugStat(), Flags(), Summary(), Keywords(), Graphic(), Charges(), Color1(), 
		Color2(), ItemType(), Field1(), Field2(), Field3(), QuestXP(), GoalText());
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

void GMsg_PostGoal::SetSummary(const TCHAR* summary)
{
 _tcsnccpy(data_.summary, summary, sizeof(data_.summary));
  TRUNC(data_.summary, sizeof(data_.summary));
}

////
// SetGoalText: copy goal text, recalc message size
////

void GMsg_PostGoal::SetGoalText(const TCHAR* goaltext)
{
 _tcsnccpy(data_.goaltext, goaltext, sizeof(data_.goaltext));
  TRUNC(data_.goaltext, sizeof(data_.goaltext));
  calc_size();
}

////
// SetKeywords
////

void GMsg_PostGoal::SetKeywords(const TCHAR* keywords)
{
 _tcsnccpy(data_.keywords, keywords, sizeof(data_.keywords));
  TRUNC(data_.keywords, sizeof(data_.keywords));
}

////
// calc_size
////

void GMsg_PostGoal::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.goaltext);
  // add string length, plus 1 for null
  size += (_tcslen(GoalText()) + 1);
  SetMessageSize(size);
}
