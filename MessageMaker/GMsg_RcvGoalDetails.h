// GMsg_RcvGoalDetails.h  -*- C++ -*-
// $Id: GMsg_RcvGoalDetails.h,v 1.8 1997-11-17 14:04:49-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// receive goal details

#ifndef INCLUDED_GMsg_RcvGoalDetails
#define INCLUDED_GMsg_RcvGoalDetails

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_RcvGoalDetails : public LmMesg {

public:

  GMsg_RcvGoalDetails();
  ~GMsg_RcvGoalDetails();

  void Init(lyra_id_t goalid, short level_num, short maxacceptances, short expirationtime,
	    short numberyes, short numberno, short voteexpiration, short statusflags, 
	    short otherflags, int num_acceptees, short num_completees,
		unsigned short graphic, unsigned char charges, 
		unsigned char color1, unsigned char color2,
		unsigned char item_type, unsigned int field1, unsigned int field2, 
		unsigned int field3, unsigned int quest_xp, 
		const TCHAR* keywords);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t GoalID() const;
  short Level() const;
  short MaxAcceptances() const;
  short ExpirationTime() const;
  short NumberYes() const;
  short NumberNo() const;
  short VoteExpiration() const;
  short StatusFlags() const;
  short OtherFlags() const;
  int NumAcceptees() const;
  short NumCompletees() const;
  unsigned short Graphic() const;
  unsigned char Charges() const;
  unsigned char Color1() const;
  unsigned char Color2() const;
  unsigned char ItemType() const;
  unsigned int Field1() const;
  unsigned int Field2() const;
  unsigned int Field3() const;
  unsigned int QuestXP() const;
  const TCHAR* Keywords() const;
  const TCHAR* Acceptee(int acceptee_num) const;

  // mutators
  void SetGoalID(lyra_id_t goalid);
  void SetLevel(short level_num);
  void SetMaxAcceptances(short maxacceptances);
  void SetExpirationTime(short expirationtime);
  void SetNumberYes(short numberyes);
  void SetNumberNo(short numberno);
  void SetVoteExpiration(short voteexpiration);
  void SetStatusFlags(short statusflags);
  void SetOtherFlags(short otherflags);
  void SetNumAcceptees(int num_acceptees);
  void SetNumCompletees(int num_completees);
  void SetGraphic(const unsigned short graphic);
  void SetCharges(const unsigned char charges);
  void SetColor1(const unsigned char color1);
  void SetColor2(const unsigned char color2);
  void SetItemType(const unsigned char item_type);
  void SetField1(const unsigned int field1);
  void SetField2(const unsigned int field2);
  void SetField3(const unsigned int field3);
  void SetQuestXP(const unsigned int quest_xp);
  void SetAcceptee(int acceptee_num, const TCHAR* name);
  void SetKeywords(const TCHAR* keywords);

  // server-only non-message data members
  lyra_id_t AccepteeID(int acceptee_num) const;
  void SetAccepteeID(int acceptee_num, lyra_id_t playerid);

public: // logically private

  // aux structures
  typedef TCHAR pname_t[Lyra::PLAYERNAME_MAX];

private:

  // standard non-public methods
  void hton();
  void ntoh();

  void calc_size();
  void calc_acceptees();

  int num_acceptees_; // number of acceptees in data_.acceptees
  lyra_id_t acceptees_[Lyra::MAX_ACCEPTS]; // store id's, names filled in later

  // message data structure
  struct data_t {
    lyra_id_t goalid;
    short level_num;
    short maxacceptances;
    short expirationtime;
    short numberyes;
    short numberno;
    short voteexpiration;
    short statusflags;
    short otherflags;
	unsigned short graphic;
	unsigned short num_completees;
	unsigned char charges;
	unsigned char color1;
	unsigned char color2;
	unsigned char item_type;
	unsigned int field1;
	unsigned int field2;
	unsigned int field3;
	unsigned int quest_xp;
	TCHAR keywords[Lyra::QUEST_KEYWORDS_LENGTH];
    pname_t acceptees[Lyra::MAX_ACCEPTS];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_RcvGoalDetails.i"
#endif

#endif /* INCLUDED_GMsg_RcvGoalDetails */



