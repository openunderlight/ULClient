// GMsg_RcvGoalDetails.i  -*- C++ -*-
// $Id: GMsg_RcvGoalDetails.i,v 1.7 1997-11-17 14:04:49-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_RcvGoalDetails::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_RcvGoalDetails::GoalID() const
{
  return data_.goalid;
}

INLINE short GMsg_RcvGoalDetails::Level() const
{
  return data_.level_num;
}

INLINE short GMsg_RcvGoalDetails::MaxAcceptances() const
{
  return data_.maxacceptances;
}

INLINE short GMsg_RcvGoalDetails::ExpirationTime() const
{
  return data_.expirationtime;
}

INLINE short GMsg_RcvGoalDetails::NumberYes() const
{
  return data_.numberyes;
}

INLINE short GMsg_RcvGoalDetails::NumberNo() const
{
  return data_.numberno;
}

INLINE short GMsg_RcvGoalDetails::VoteExpiration() const
{
  return data_.voteexpiration;
}

INLINE int GMsg_RcvGoalDetails::NumAcceptees() const
{
  return num_acceptees_;
}

INLINE short GMsg_RcvGoalDetails::NumCompletees() const
{
  return data_.num_completees;
}

INLINE const TCHAR* GMsg_RcvGoalDetails::Acceptee(int acceptee_num) const
{
  return data_.acceptees[acceptee_num];
}

INLINE const TCHAR* GMsg_RcvGoalDetails::Keywords() const
{
  return data_.keywords;
}

INLINE short GMsg_RcvGoalDetails::StatusFlags() const
{
  return data_.statusflags;
}

INLINE short GMsg_RcvGoalDetails::OtherFlags() const
{
  return data_.otherflags;
}

INLINE unsigned short GMsg_RcvGoalDetails::Graphic() const
{
  return data_.graphic;
}

INLINE unsigned char GMsg_RcvGoalDetails::Charges() const
{
  return data_.charges;
}

INLINE unsigned char GMsg_RcvGoalDetails::Color1() const
{
  return data_.color1;
}

INLINE unsigned char GMsg_RcvGoalDetails::Color2() const
{
  return data_.color2;
}

INLINE unsigned char GMsg_RcvGoalDetails::ItemType() const
{
  return data_.item_type;
}

INLINE unsigned int GMsg_RcvGoalDetails::Field1() const
{
  return data_.field1;
}

INLINE unsigned int GMsg_RcvGoalDetails::Field2() const
{
  return data_.field2;
}

INLINE unsigned int GMsg_RcvGoalDetails::Field3() const
{
  return data_.field3;
}

INLINE unsigned int GMsg_RcvGoalDetails::QuestXP() const
{
  return data_.quest_xp;
}

INLINE void GMsg_RcvGoalDetails::SetGoalID(lyra_id_t goalid)
{
  data_.goalid = goalid;
}

INLINE void GMsg_RcvGoalDetails::SetLevel(short level_num)
{
  data_.level_num = level_num;
}

INLINE void GMsg_RcvGoalDetails::SetMaxAcceptances(short maxacceptances)
{
  data_.maxacceptances = maxacceptances;
}

INLINE void GMsg_RcvGoalDetails::SetExpirationTime(short expirationtime)
{
  data_.expirationtime = expirationtime;
}

INLINE void GMsg_RcvGoalDetails::SetNumberYes(short numberyes)
{
  data_.numberyes = numberyes;
}

INLINE void GMsg_RcvGoalDetails::SetNumberNo(short numberno)
{
  data_.numberno = numberno;
}

INLINE void GMsg_RcvGoalDetails::SetVoteExpiration(short voteexpiration)
{
  data_.voteexpiration = voteexpiration;
}

INLINE void GMsg_RcvGoalDetails::SetStatusFlags(short statusflags)
{
  data_.statusflags = statusflags;
}

INLINE void GMsg_RcvGoalDetails::SetOtherFlags(short otherflags)
{
  data_.otherflags = otherflags;
}

INLINE lyra_id_t GMsg_RcvGoalDetails::AccepteeID(int acceptee_num) const
{
  return acceptees_[acceptee_num];
}

INLINE void GMsg_RcvGoalDetails::SetAccepteeID(int acceptee_num, lyra_id_t playerid)
{
  acceptees_[acceptee_num] = playerid;
}

INLINE void GMsg_RcvGoalDetails::SetGraphic(unsigned short graphic)
{
  data_.graphic = graphic;
}

INLINE void GMsg_RcvGoalDetails::SetCharges(unsigned char charges)
{
  data_.charges = charges;
}

INLINE void GMsg_RcvGoalDetails::SetColor1(unsigned char color1)
{
  data_.color1 = color1;
}

INLINE void GMsg_RcvGoalDetails::SetColor2(unsigned char color2)
{
  data_.color2 = color2;
}

INLINE void GMsg_RcvGoalDetails::SetItemType(unsigned char item_type)
{
  data_.item_type = item_type;
}

INLINE void GMsg_RcvGoalDetails::SetField1(unsigned int field1)
{
  data_.field1 = field1;
}

INLINE void GMsg_RcvGoalDetails::SetField2(unsigned int field2)
{
  data_.field2 = field2;
}

INLINE void GMsg_RcvGoalDetails::SetField3(unsigned int field3)
{
  data_.field3 = field3;
}

INLINE void GMsg_RcvGoalDetails::SetQuestXP(unsigned int quest_xp)
{
  data_.quest_xp = quest_xp;
}

