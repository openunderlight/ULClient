// LmAvatar.h  -*- C++ -*-
// $Id: LmAvatar.h,v 1.10 1998-02-12 15:33:11-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// avatar type

#ifndef INCLUDED_LmAvatar
#define INCLUDED_LmAvatar

#include <stdio.h>

#include "LyraDefs.h"
#include "LmBit32.h"

// base type

struct lyra_avatar_t {
  // BIT31 unused:1 show_guild:2 guild_rank:2 guild_id:3 color4:4 color3:4 color2:4 color1:4 color0:4 bitmap_type:4 BIT0
  LmBit32 avatar1; 
  // BIT31 unused:5 np symbol: 1 dreamstrike: 1 wordsmith:1 teacher:1 focus:3 show_sphere:2 sphere:4 head:4 show_lyran:1 master_teacher:1 account:3 hidden:1 GK xtra damage: 3
  // dreamsmith:1 BIT0
  LmBit32 avatar2; 
}; // total = 64 bits, 8 bytes

// the avatar class

class LmAvatar {

public:

	enum {
		// bitfield widths for avatar1
		AVATAR_WIDTH = 4,
		COLOR_WIDTH = 4, // 5 of these
		GUILD_ID_WIDTH = 4,
		GUILD_RANK_WIDTH = 2,
		SHOW_GUILD_WIDTH = 2,

		// bitfield widths for avatar2
		HEAD_WIDTH = 4,
		SPHERE_WIDTH = 4,
		SHOW_SPHERE_WIDTH = 2,
		TEACHER_WIDTH = 1,
		FOCUS_WIDTH = 3,
		MASTER_TEACHER_WIDTH = 1,
		SHOW_LYRAN_WIDTH = 1,
		DREAMSMITH_WIDTH = 1,
		ACCOUNT_WIDTH = 3,
		HIDDEN_WIDTH = 1,
		EXTRA_DAMAGE_WIDTH = 4,
		WORDSMITH_WIDTH = 1,
		DREAMSTRIKE_WIDTH = 1,
		NP_SYMBOL_WIDTH = 1,
		APPRENTICE_WIDTH = 1,
		UNUSED2_WIDTH = 3,
    
    // starting positions for avatar1
    AVATAR_START = 0,
    COLOR0_START = (AVATAR_START + AVATAR_WIDTH),
    COLOR1_START = (COLOR0_START + COLOR_WIDTH),
    COLOR2_START = (COLOR1_START + COLOR_WIDTH),
    COLOR3_START = (COLOR2_START + COLOR_WIDTH),
    COLOR4_START = (COLOR3_START + COLOR_WIDTH),
    GUILD_ID_START = (COLOR4_START + COLOR_WIDTH),
    GUILD_RANK_START = (GUILD_ID_START + GUILD_ID_WIDTH),
    SHOW_GUILD_START = (GUILD_RANK_START + GUILD_RANK_WIDTH),

    // starting positions for avatar2
    HEAD_START = 0, 
    SPHERE_START = (HEAD_START + HEAD_WIDTH),
    SHOW_SPHERE_START = (SPHERE_START + SPHERE_WIDTH),
    TEACHER_START = (SHOW_SPHERE_START + SHOW_SPHERE_WIDTH),
	FOCUS_START = (TEACHER_START + TEACHER_WIDTH),
	MASTER_TEACHER_START = (FOCUS_START + FOCUS_WIDTH),
	SHOW_LYRAN_START = (MASTER_TEACHER_START + MASTER_TEACHER_WIDTH),
	DREAMSMITH_START = (SHOW_LYRAN_START + SHOW_LYRAN_WIDTH),
	ACCOUNT_START = (DREAMSMITH_START + DREAMSMITH_WIDTH),
	HIDDEN_START = (ACCOUNT_START + ACCOUNT_WIDTH),
	EXTRA_DAMAGE_START = (HIDDEN_START + HIDDEN_WIDTH),
	WORDSMITH_START = (EXTRA_DAMAGE_START + EXTRA_DAMAGE_WIDTH),
	DREAMSTRIKE_START = (WORDSMITH_START + WORDSMITH_WIDTH),
	NP_SYMBOL_START = (DREAMSTRIKE_START + DREAMSTRIKE_WIDTH),
	APPRENTICE_START = (NP_SYMBOL_START + NP_SYMBOL_WIDTH),
	// account types for use by client
	ACCT_DREAMER = 0,
	ACCT_PMARE = 1,
	ACCT_NIGHTMARE = 2,
	ACCT_DARKMARE = 3,
	ACCT_ADMIN = 4

  };
  
  static const LmAvatar DEFAULT_INSTANCE;

public:

  LmAvatar();

  void Init(unsigned int avatar_type, unsigned int color0, 
	    unsigned int color1, unsigned int color2,
	    unsigned int color3, unsigned int color4,
	    unsigned int guild_id, unsigned int guild_rank,
	    unsigned int show_guild, unsigned int head_type, 
	    unsigned int sphere, unsigned int show_sphere, 
	    unsigned int teacher, unsigned int master_teacher, 
		unsigned int show_lyran, unsigned int dreamsmith, 
		unsigned int hidden, unsigned int extra_damage, 
		unsigned int wordsmith, unsigned int dreamstrike,
		unsigned int focus, unsigned int np_symbol, unsigned int apprentice);
  void Init(int avatar1, int avatar2);
  void Init(const lyra_avatar_t& avatar);

  // selectors
  const lyra_avatar_t& Avatar() const;
  int Avatar1() const;
  int Avatar2() const;
  int BitmapID() const; // returns base 4dx bitmap id for this avatar
  int EssenceBitmap() const; // returns essence bitmap id for this avatar

  // mutators
  void SetAvatar(const lyra_avatar_t& avatar);
  void SetAvatar1(int avatar1);
  void SetAvatar2(int avatar2);
  void SetBitmapID(unsigned int bitmap_id); // sets type based on 4dx bitmap id

  // selectors for bitfields
  unsigned int AvatarType() const;
  unsigned int Color0() const;
  unsigned int Color1() const;
  unsigned int Color2() const;
  unsigned int Color3() const;
  unsigned int Color4() const;
  unsigned int GuildID() const;
  unsigned int GuildRank() const;
  unsigned int ShowGuild() const;
  unsigned int Head() const;
  unsigned int Sphere() const;
  unsigned int ShowSphere() const;
  unsigned int Teacher() const;
  unsigned int Focus() const;
  unsigned int MasterTeacher() const;
  unsigned int ShowLyran() const;
  unsigned int DreamSmith() const;
  unsigned int AccountType() const;
  unsigned int Hidden() const;
  unsigned int ExtraDamage() const;
  unsigned int WordSmith() const;
  unsigned int Dreamstrike() const;
  unsigned int NPSymbol() const;
  unsigned int Apprentice() const;

  // mutators for bitfields
  void SetAvatarType(unsigned int type);
  void SetColor0(unsigned int color);
  void SetColor1(unsigned int color);
  void SetColor2(unsigned int color);
  void SetColor3(unsigned int color);
  void SetColor4(unsigned int color);
  void SetGuildID(unsigned int id);
  void SetGuildRank(unsigned int rank);
  void SetShowGuild(unsigned int show);
  void SetHead(unsigned int id);
  void SetSphere(unsigned int sphere);
  void SetShowSphere(unsigned int show);
  void SetTeacher(unsigned int teacher);
  void SetFocus(unsigned int focus);
  void SetMasterTeacher(unsigned int master_teacher);
  void SetShowLyran(unsigned int show_lyran);
  void SetDreamSmith(unsigned int dreamsmith);
  void SetAccountType(unsigned int acct_type);
  void SetHidden(unsigned int hidden);
  void SetExtraDamage(unsigned int extra_damage);
  void SetWordSmith(unsigned int wordsmith);
  void SetDreamstrike(unsigned int dreamstrike);
  void SetNPSymbol(unsigned int np_symbol);
  void SetApprentice(unsigned int apprentice);

  // read from string, write to string
  int Parse(const TCHAR* str);
  void UnParse(TCHAR* str, int strlen) const;

  // byteorder conversion
  void ConvertToHost();
  void ConvertToNetwork();

  void Dump(FILE* f, int indent = 0) const;
  void Dump1(FILE* f) const;

private:

  lyra_avatar_t avatar_;

};

#ifdef USE_INLINE
#include "LmAvatar.i"
#endif

// inline functions

inline int operator==(const LmAvatar& a1, const LmAvatar& a2)
{
  return ((a1.AvatarType() == a2.AvatarType()) &&
	  (a1.Color0() == a2.Color0()) &&
	  (a1.Color1() == a2.Color1()) &&
	  (a1.Color2() == a2.Color2()) &&
	  (a1.Color3() == a2.Color3()) &&
	  (a1.Color4() == a2.Color4()) &&
	  (a1.GuildID() == a2.GuildID()) &&
	  (a1.GuildRank() == a2.GuildRank()) &&
	  (a1.ShowGuild() == a2.ShowGuild()) &&
	  (a1.Head() == a2.Head()) &&
	  (a1.ShowSphere() == a2.ShowSphere()) &&
	  (a1.Sphere() == a2.Sphere()) &&
	  (a1.Teacher() == a2.Teacher()) &&
	  (a1.Apprentice() == a2.Apprentice()) &&
	  (a1.MasterTeacher() == a2.MasterTeacher()) &&
	  (a1.ShowLyran() == a2.ShowLyran()) &&
	  (a1.DreamSmith() == a2.DreamSmith()) &&
	  (a1.AccountType() == a2.AccountType()) && 
	  (a1.Hidden() == a2.Hidden()) &&
	  (a1.ExtraDamage() == a2.ExtraDamage()) &&
	  (a1.WordSmith() == a2.WordSmith())  &&
	  (a1.Dreamstrike() == a2.Dreamstrike())&&
	  (a1.Focus() == a2.Focus()));
}

#endif /* INCLUDED_LmAvatar */
