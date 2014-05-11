// GMsg_PostGoal.i  -*- C++ -*-
// $Id: GMsg_PostGoal.i,v 1.8 1997-11-17 14:04:49-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_PostGoal::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_PostGoal::GoalID() const
{
  return data_.goalid;
}

INLINE short GMsg_PostGoal::Level() const
{
  return data_.level_num;
}

INLINE short GMsg_PostGoal::Guild() const
{
  return data_.guild;
}

INLINE short GMsg_PostGoal::MaxAccepted() const
{
  return data_.maxaccepted;
}

INLINE short GMsg_PostGoal::ExpirationTime() const
{
  return data_.expirationtime;
}

INLINE short GMsg_PostGoal::VoteExpiration() const
{
  return data_.voteexpire;
}

INLINE short GMsg_PostGoal::SugSphere() const
{
  return data_.sugsphere;
}

INLINE short GMsg_PostGoal::SugStat() const
{
  return data_.sugstat;
}

INLINE short GMsg_PostGoal::Flags() const
{
  return data_.flags;
}

INLINE unsigned short GMsg_PostGoal::Graphic() const
{
  return data_.graphic;
}

INLINE unsigned char GMsg_PostGoal::Charges() const
{
  return data_.charges;
}


INLINE unsigned char GMsg_PostGoal::Color1() const
{
  return data_.color1;
}

INLINE unsigned char GMsg_PostGoal::Color2() const
{
  return data_.color2;
}

INLINE unsigned char GMsg_PostGoal::ItemType() const
{
  return data_.item_type;
}

INLINE unsigned int GMsg_PostGoal::Field1() const
{
  return data_.field1;
}

INLINE unsigned int GMsg_PostGoal::Field2() const
{
  return data_.field2;
}

INLINE unsigned int GMsg_PostGoal::Field3() const
{
  return data_.field3;
}


INLINE unsigned int GMsg_PostGoal::QuestXP() const
{
  return data_.quest_xp;
}

INLINE const TCHAR* GMsg_PostGoal::Summary() const
{
  return data_.summary;
}

INLINE const TCHAR* GMsg_PostGoal::Keywords() const
{
  return data_.keywords;
}

INLINE const TCHAR* GMsg_PostGoal::GoalText() const
{
  return data_.goaltext;
}

INLINE void GMsg_PostGoal::SetGoalID(lyra_id_t goalid)
{
  data_.goalid = goalid;
}

INLINE void GMsg_PostGoal::SetLevel(short level_num)
{
  data_.level_num = level_num;
}

INLINE void GMsg_PostGoal::SetGuild(short guild)
{
  data_.guild = guild;
}

INLINE void GMsg_PostGoal::SetMaxAccepted(short maxaccepted)
{
  data_.maxaccepted = maxaccepted;
}

INLINE void GMsg_PostGoal::SetExpirationTime(short expirationtime)
{
  data_.expirationtime = expirationtime;
}

INLINE void GMsg_PostGoal::SetVoteExpiration(short voteexpire)
{
  data_.voteexpire = voteexpire;
}

INLINE void GMsg_PostGoal::SetSugSphere(short sugsphere)
{
  data_.sugsphere = sugsphere;
}

INLINE void GMsg_PostGoal::SetSugStat(short sugstat)
{
  data_.sugstat = sugstat;
}

INLINE void GMsg_PostGoal::SetFlags(short flags)
{
  data_.flags = flags;
}

INLINE void GMsg_PostGoal::SetGraphic(unsigned short graphic)
{
  data_.graphic = graphic;
}

INLINE void GMsg_PostGoal::SetCharges(unsigned char charges)
{
  data_.charges = charges;
}

INLINE void GMsg_PostGoal::SetColor1(unsigned char color1)
{
  data_.color1 = color1;
}

INLINE void GMsg_PostGoal::SetColor2(unsigned char color2)
{
  data_.color2 = color2;
}

INLINE void GMsg_PostGoal::SetItemType(unsigned char item_type)
{
  data_.item_type = item_type;
}

INLINE void GMsg_PostGoal::SetField1(unsigned int field1)
{
  data_.field1 = field1;
}

INLINE void GMsg_PostGoal::SetField2(unsigned int field2)
{
  data_.field2 = field2;
}

INLINE void GMsg_PostGoal::SetField3(unsigned int field3)
{
  data_.field3 = field3;
}

INLINE void GMsg_PostGoal::SetQuestXP(unsigned int quest_xp)
{
  data_.quest_xp = quest_xp;
}


