// LmAvatar.i  -*- C++ -*-
// $Id: LmAvatar.i,v 1.8 1998-02-12 15:33:11-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions

#include "SharedConstants.h"

#ifndef USE_DEBUG
INLINE void LmAvatar::Dump(FILE*, int) const
{
  // empty
}

INLINE void LmAvatar::Dump1(FILE*) const
{
  // empty
}
#endif /* !USE_DEBUG */

// returns base 4dx bitmap id for this avatar
INLINE int LmAvatar::BitmapID() const
{
  switch (this->AvatarType()) {
  case Avatars::MALE:
    return LyraBitmap::MALE_AVATAR;

  case Avatars::FEMALE:
    return LyraBitmap::FEMALE_AVATAR;

  case Avatars::EMPHANT:
    return LyraBitmap::EMPHANT_AVATAR;

  case Avatars::BOGROM:
    return LyraBitmap::BOGROM_AVATAR;

  case Avatars::AGOKNIGHT:
    return LyraBitmap::AGOKNIGHT_AVATAR;

  case Avatars::SHAMBLIX:
    return LyraBitmap::SHAMBLIX_AVATAR;

  case Avatars::HORRON:
    return LyraBitmap::HORRON_AVATAR;

  default:
    return LyraBitmap::MALE_AVATAR;
  }
}

INLINE int LmAvatar::EssenceBitmap() const
{
  switch (this->AvatarType()) {
  case Avatars::MALE:
    return LyraBitmap::M_AVATAR_ESSENCE;

  case Avatars::FEMALE:
    return LyraBitmap::F_AVATAR_ESSENCE;

  case Avatars::EMPHANT:
    return LyraBitmap::EMPHANT_ESSENCE;

  case Avatars::BOGROM:
    return LyraBitmap::BOGROM_ESSENCE;

  case Avatars::AGOKNIGHT:
    return LyraBitmap::AGOKNIGHT_ESSENCE;

  case Avatars::SHAMBLIX:
  case Avatars::HORRON: // no Horron essence yet
    return LyraBitmap::SHAMBLIX_ESSENCE;

  default:
    return LyraBitmap::M_AVATAR_ESSENCE;
  }
}

// converts 4dx bitmap id's into avatar types
INLINE void LmAvatar::SetBitmapID(unsigned int bitmap_id)
{
  switch (bitmap_id) {
  case LyraBitmap::MALE_AVATAR:
    this->SetAvatarType(Avatars::MALE);
    break;

  case LyraBitmap::FEMALE_AVATAR:	
    this->SetAvatarType(Avatars::FEMALE);
    break;  	

  case LyraBitmap::EMPHANT_AVATAR:
    this->SetAvatarType(Avatars::EMPHANT);
    break;

  case LyraBitmap::BOGROM_AVATAR:
    this->SetAvatarType(Avatars::BOGROM);
    break;

  case LyraBitmap::AGOKNIGHT_AVATAR:
    this->SetAvatarType(Avatars::AGOKNIGHT);
    break;

  case LyraBitmap::SHAMBLIX_AVATAR:
    this->SetAvatarType(Avatars::SHAMBLIX);
    break;

/*
  case LyraBitmap::HORRON_AVATAR:
    this->SetAvatarType(Avatars::SHAMBLIX);
    break;
	*/

  default:
    this->SetAvatarType(Avatars::EMPHANT);
    break;
  }
}

INLINE const lyra_avatar_t& LmAvatar::Avatar() const
{
  return avatar_;
}

INLINE int LmAvatar::Avatar1() const
{
  return avatar_.avatar1.Value();
}

INLINE int LmAvatar::Avatar2() const
{
  return avatar_.avatar2.Value();
}

INLINE void LmAvatar::SetAvatar1(int avatar1)
{
  avatar_.avatar1.SetValue(avatar1);
}

INLINE void LmAvatar::SetAvatar2(int avatar2)
{
  avatar_.avatar2.SetValue(avatar2);
}

INLINE unsigned int LmAvatar::AvatarType() const
{
  return avatar_.avatar1.GetBits(AVATAR_START, AVATAR_WIDTH);
}

INLINE unsigned int LmAvatar::Color0() const
{
  return avatar_.avatar1.GetBits(COLOR0_START, COLOR_WIDTH);
}

INLINE unsigned int LmAvatar::Color1() const
{
  return avatar_.avatar1.GetBits(COLOR1_START, COLOR_WIDTH);
}

INLINE unsigned int LmAvatar::Color2() const
{
  return avatar_.avatar1.GetBits(COLOR2_START, COLOR_WIDTH);
}

INLINE unsigned int LmAvatar::Color3() const
{
  return avatar_.avatar1.GetBits(COLOR3_START, COLOR_WIDTH);
}

INLINE unsigned int LmAvatar::Color4() const
{
  return avatar_.avatar1.GetBits(COLOR4_START, COLOR_WIDTH);
}

INLINE unsigned int LmAvatar::GuildID() const
{
  return avatar_.avatar1.GetBits(GUILD_ID_START, GUILD_ID_WIDTH);
}

INLINE unsigned int LmAvatar::GuildRank() const
{
  return avatar_.avatar1.GetBits(GUILD_RANK_START, GUILD_RANK_WIDTH);
}

INLINE unsigned int LmAvatar::ShowGuild() const
{
  return avatar_.avatar1.GetBits(SHOW_GUILD_START, SHOW_GUILD_WIDTH);
}

INLINE unsigned int LmAvatar::Head() const
{
  return avatar_.avatar2.GetBits(HEAD_START, HEAD_WIDTH);
}

INLINE unsigned int LmAvatar::Sphere() const
{
  return avatar_.avatar2.GetBits(SPHERE_START, SPHERE_WIDTH);
}

INLINE unsigned int LmAvatar::ShowSphere() const
{
  return avatar_.avatar2.GetBits(SHOW_SPHERE_START, SHOW_SPHERE_WIDTH);
}

INLINE unsigned int LmAvatar::Teacher() const
{
  return avatar_.avatar2.GetBits(TEACHER_START, TEACHER_WIDTH);
}

INLINE unsigned int LmAvatar::Focus() const
{
  return avatar_.avatar2.GetBits(FOCUS_START, FOCUS_WIDTH);
}

INLINE unsigned int LmAvatar::MasterTeacher() const
{
  return avatar_.avatar2.GetBits(MASTER_TEACHER_START, MASTER_TEACHER_WIDTH);
}

INLINE unsigned int LmAvatar::ShowLyran() const
{
  return avatar_.avatar2.GetBits(SHOW_LYRAN_START, SHOW_LYRAN_WIDTH);
}

INLINE unsigned int LmAvatar::DreamSmith() const
{
  return avatar_.avatar2.GetBits(DREAMSMITH_START, DREAMSMITH_WIDTH);
}

INLINE unsigned int LmAvatar::WordSmith() const
{
  return avatar_.avatar2.GetBits(WORDSMITH_START, WORDSMITH_WIDTH);
}

INLINE unsigned int LmAvatar::Dreamstrike() const
{
  return avatar_.avatar2.GetBits(DREAMSTRIKE_START, DREAMSTRIKE_WIDTH);
}


INLINE unsigned int LmAvatar::NPSymbol() const
{
  return avatar_.avatar2.GetBits(NP_SYMBOL_START, NP_SYMBOL_WIDTH);
}


INLINE unsigned int LmAvatar::AccountType() const
{
  return avatar_.avatar2.GetBits(ACCOUNT_START, ACCOUNT_WIDTH);
}

INLINE unsigned int LmAvatar::Hidden() const
{
  return avatar_.avatar2.GetBits(HIDDEN_START, HIDDEN_WIDTH);
}

INLINE unsigned int LmAvatar::ExtraDamage() const
{
  return avatar_.avatar2.GetBits(EXTRA_DAMAGE_START, EXTRA_DAMAGE_WIDTH);
}

INLINE void LmAvatar::SetAvatar(const lyra_avatar_t& avatar)
{
  avatar_ = avatar;
}

INLINE void LmAvatar::SetAvatarType(unsigned int type)
{
  avatar_.avatar1.SetBits(AVATAR_START, AVATAR_WIDTH, type);
}

INLINE void LmAvatar::SetColor0(unsigned int color)
{
  avatar_.avatar1.SetBits(COLOR0_START, COLOR_WIDTH, color);
}

INLINE void LmAvatar::SetColor1(unsigned int color)
{
  avatar_.avatar1.SetBits(COLOR1_START, COLOR_WIDTH, color);
}

INLINE void LmAvatar::SetColor2(unsigned int color)
{
  avatar_.avatar1.SetBits(COLOR2_START, COLOR_WIDTH, color);
}

INLINE void LmAvatar::SetColor3(unsigned int color)
{
  avatar_.avatar1.SetBits(COLOR3_START, COLOR_WIDTH, color);
}

INLINE void LmAvatar::SetColor4(unsigned int color)
{
  avatar_.avatar1.SetBits(COLOR4_START, COLOR_WIDTH, color);
}

INLINE void LmAvatar::SetGuildID(unsigned int id)
{
  avatar_.avatar1.SetBits(GUILD_ID_START, GUILD_ID_WIDTH, id);
}

INLINE void LmAvatar::SetGuildRank(unsigned int rank)
{
  avatar_.avatar1.SetBits(GUILD_RANK_START, GUILD_RANK_WIDTH, rank);
}

INLINE void LmAvatar::SetShowGuild(unsigned int show)
{
  avatar_.avatar1.SetBits(SHOW_GUILD_START, SHOW_GUILD_WIDTH, show);
}

INLINE void LmAvatar::SetHead(unsigned int id)
{
  avatar_.avatar2.SetBits(HEAD_START, HEAD_WIDTH, id);
}

INLINE void LmAvatar::SetSphere(unsigned int sphere)
{
  avatar_.avatar2.SetBits(SPHERE_START, SPHERE_WIDTH, sphere);
}

INLINE void LmAvatar::SetShowSphere(unsigned int show)
{
  avatar_.avatar2.SetBits(SHOW_SPHERE_START, SHOW_SPHERE_WIDTH, show);
}

INLINE void LmAvatar::SetTeacher(unsigned int teacher)
{
  avatar_.avatar2.SetBits(TEACHER_START, TEACHER_WIDTH, teacher);
}

INLINE void LmAvatar::SetFocus(unsigned int focus)
{
  avatar_.avatar2.SetBits(FOCUS_START, FOCUS_WIDTH, focus);
}

INLINE void LmAvatar::SetMasterTeacher(unsigned int master_teacher)
{
  avatar_.avatar2.SetBits(MASTER_TEACHER_START, MASTER_TEACHER_WIDTH, master_teacher);
}

INLINE void LmAvatar::SetShowLyran(unsigned int show_lyran)
{
  avatar_.avatar2.SetBits(SHOW_LYRAN_START, SHOW_LYRAN_WIDTH, show_lyran);
}

INLINE void LmAvatar::SetDreamSmith(unsigned int dreamsmith)
{
  avatar_.avatar2.SetBits(DREAMSMITH_START, DREAMSMITH_WIDTH, dreamsmith);
}

INLINE void LmAvatar::SetWordSmith(unsigned int wordsmith)
{
  avatar_.avatar2.SetBits(WORDSMITH_START, WORDSMITH_WIDTH, wordsmith);
}

INLINE void LmAvatar::SetDreamstrike(unsigned int dreamstrike)
{
  avatar_.avatar2.SetBits(DREAMSTRIKE_START, DREAMSTRIKE_WIDTH, dreamstrike);
}

INLINE void LmAvatar::SetNPSymbol(unsigned int np_symbol)
{
  avatar_.avatar2.SetBits(NP_SYMBOL_START, NP_SYMBOL_WIDTH, np_symbol);
}

INLINE void LmAvatar::SetAccountType(unsigned int acct_type)
{
  avatar_.avatar2.SetBits(ACCOUNT_START, ACCOUNT_WIDTH, acct_type);
}

INLINE void LmAvatar::SetHidden(unsigned int hidden)
{
  avatar_.avatar2.SetBits(HIDDEN_START, HIDDEN_WIDTH, hidden);
}

INLINE void LmAvatar::SetExtraDamage(unsigned int extra_damage)
{
  avatar_.avatar2.SetBits(EXTRA_DAMAGE_START, EXTRA_DAMAGE_WIDTH, extra_damage);
}


