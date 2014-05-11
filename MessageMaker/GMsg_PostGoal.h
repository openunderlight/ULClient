// GMsg_PostGoal.h  -*- C++ -*-
// $Id: GMsg_PostGoal.h,v 1.9 1997-11-17 14:04:49-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// post a new goal

#ifndef INCLUDED_GMsg_PostGoal
#define INCLUDED_GMsg_PostGoal

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_PostGoal : public LmMesg {

public:

  GMsg_PostGoal();
  ~GMsg_PostGoal();


  void Init(lyra_id_t goalid, short level_num, short guild, short maxaccepted, 
	    short expirationtime, short sugsphere, short sugstat, short flags,
	    const TCHAR* summary, const TCHAR* goaltext, short voteexpire,
		unsigned short graphic, unsigned char charges, unsigned char color1, unsigned char color2,
		unsigned char item_type, unsigned int field1, unsigned int field2, 
		unsigned int field3, unsigned int quest_xp, const TCHAR* keywords);
		
  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t GoalID() const;
  short Level() const;
  short Guild() const;
  short MaxAccepted() const;
  short ExpirationTime() const;
  short VoteExpiration() const;
  short SugSphere() const;
  short SugStat() const;
  short Flags() const;
  const TCHAR* Summary() const;
  const TCHAR* GoalText() const;
  const TCHAR* Keywords() const;
  unsigned short Graphic() const;
  unsigned char Charges() const;
  unsigned char Color1() const;
  unsigned char Color2() const;
  unsigned char ItemType() const;
  unsigned int Field1() const;
  unsigned int Field2() const;
  unsigned int Field3() const;
  unsigned int QuestXP() const;

  // mutators
  void SetGoalID(lyra_id_t goalid);
  void SetLevel(short level_num);
  void SetGuild(short guild);
  void SetMaxAccepted(short maxaccepted);
  void SetExpirationTime(short expirationtime);
  void SetVoteExpiration(short voteexpire);
  void SetSugSphere(short sugsphere);
  void SetSugStat(short sugstat);
  void SetFlags(short flags);
  void SetSummary(const TCHAR* summary);
  void SetGoalText(const TCHAR* goaltext);
  void SetKeywords(const TCHAR* keywords);
  void SetGraphic(const unsigned short graphic);
  void SetCharges(const unsigned char charges);
  void SetColor1(const unsigned char color1);
  void SetColor2(const unsigned char color2);
  void SetItemType(const unsigned char item_type);
  void SetField1(const unsigned int field1);
  void SetField2(const unsigned int field2);
  void SetField3(const unsigned int field3);
  void SetQuestXP(const unsigned int quest_xp);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual message size
  void calc_size();

  // message data structure
  struct data_t {
    lyra_id_t goalid;
    short level_num;
    short guild;
    short maxaccepted;
    short voteexpire;
    short sugsphere;
    short sugstat;
    short expirationtime;
    short flags;
	unsigned short graphic;
	unsigned short padding;
	unsigned char charges;
	unsigned char color1;
	unsigned char color2;
	unsigned char item_type;
	unsigned int field1;
	unsigned int field2;
	unsigned int field3;
	unsigned int quest_xp;
    TCHAR summary[Lyra::GOAL_SUMMARY_LENGTH];
	TCHAR keywords[Lyra::QUEST_KEYWORDS_LENGTH];
    TCHAR goaltext[Lyra::MAX_GOAL_LENGTH];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_PostGoal.i"
#endif

#endif /* INCLUDED_GMsg_PostGoal */
