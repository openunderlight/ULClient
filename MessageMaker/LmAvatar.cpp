// LmAvatar.cpp  -*- C++ -*-
// $Id: LmAvatar.cpp,v 1.8 1998-04-03 17:01:13-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#endif
#include <stdio.h>
#include <string.h>

#include "SharedConstants.h"
#include "LmAvatar.h"

#ifndef USE_INLINE
#include "LmAvatar.i"
#endif

// default object
const LmAvatar LmAvatar::DEFAULT_INSTANCE;

////
// constructor
////

LmAvatar::LmAvatar()
{
	Init(Avatars::MALE, 0, 0, 0, 0, 0, Guild::NO_GUILD, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

////
// Init
////

void LmAvatar::Init(unsigned int avatar_type, unsigned int color0, 
		    unsigned int color1, unsigned int color2,
		    unsigned int color3, unsigned int color4,
			unsigned int guild_id, unsigned int guild_rank,
			unsigned int show_guild, unsigned int head_type, 
			unsigned int sphere, unsigned int show_sphere, 
			unsigned int teacher, unsigned int master_teacher, 
			unsigned int show_lyran, unsigned int dreamsmith,
			unsigned int hidden, unsigned int extra_damage,
			unsigned int wordsmith, unsigned int dreamstrike,
			unsigned int focus, unsigned int np_symbol
			)
{
  SetAvatarType(avatar_type);
  SetColor0(color0);
  SetColor1(color1);
  SetColor2(color2);
  SetColor3(color3);
  SetColor4(color4);
  SetGuildID(guild_id);
  SetGuildRank(guild_rank);
  SetShowGuild(show_guild);
  SetHead(head_type);
  SetSphere(sphere);
  SetShowSphere(show_sphere);
  SetTeacher(teacher);
  SetMasterTeacher(master_teacher);
  SetShowLyran(show_lyran);
  SetDreamSmith(dreamsmith);
  SetAccountType(LmAvatar::ACCT_DREAMER); // default to dreamer - set by server
  SetHidden(hidden);
  SetExtraDamage(extra_damage);
  SetWordSmith(wordsmith);
  SetDreamstrike(dreamstrike);
  SetFocus(focus);
  SetNPSymbol(np_symbol);
}

void LmAvatar::Init(const lyra_avatar_t& avatar)
{
  SetAvatar(avatar);
}

void LmAvatar::Init(int avatar1, int avatar2)
{
  SetAvatar1(avatar1);
  SetAvatar2(avatar2);
}

////
// ConvertToHost
////

void LmAvatar::ConvertToHost()
{
  avatar_.avatar1.ConvertToHost();
  avatar_.avatar2.ConvertToHost();
}

////
// ConvertToNetwork
////

void LmAvatar::ConvertToNetwork()
{
  avatar_.avatar1.ConvertToNetwork();
  avatar_.avatar2.ConvertToNetwork();
}

////
// Parse
////

int LmAvatar::Parse(const TCHAR* str)
{
  int av1, av2;
  if (_stscanf(str, _T("%u:%u"), &av1, &av2) != 2) {
    return -1;
  }
  SetAvatar1(av1);
  SetAvatar2(av2);
  return 0;
}

////
// UnParse
////

void LmAvatar::UnParse(TCHAR* str, int /* strlen */) const
{
 _stprintf(str, _T("%u:%u"), Avatar1(), Avatar2());
}

////
// Dump
////

#ifdef USE_DEBUG
void LmAvatar::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmAvatar[%p,%d]: av=[%u:%u] bm=%u colors=(%u,%u,%u,%u,%u) guild_id=%u guild_rank=%u show_guild=%u head=%u sphere=%u show_sphere=%u teacher=%u master teacher=%u show lyran=%u dreamsmith=%u acct_type=%u hidden=%u extra_damage=%u wordsmith=%u dreamstrike=%u focus=%u np_symbol=%u>\n"),
	  this, sizeof(LmAvatar),
	  Avatar1(), Avatar2(), AvatarType(), Color0(), Color1(), Color2(), 
	  Color3(), Color4(), GuildID(), GuildRank(), ShowGuild(), Head(), 
	  Sphere(), ShowSphere(), Teacher(), MasterTeacher(), ShowLyran(), 
	  DreamSmith(), AccountType(), Hidden(), ExtraDamage(), WordSmith(), 
	  Dreamstrike(), Focus(), NPSymbol());
}
#endif /* USE_DEBUG */

////
// Dump1
////

#ifdef USE_DEBUG
void LmAvatar::Dump1(FILE* f) const
{
 _ftprintf(f, _T("[%u:%u]"), Avatar1(), Avatar2());
}
#endif /* USE_DEBUG */
