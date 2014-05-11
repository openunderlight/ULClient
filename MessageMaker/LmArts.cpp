// LmArts.cpp  -*- C++ -*-
// $Id: LmArts.cpp,v 1.3 1997-07-29 18:15:00-07 jason Exp $
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

#include "LmArts.h"

#ifndef USE_INLINE
#include "LmArts.i"
#endif

////
// constructor
////

LmArts::LmArts()
{
  Init();
}

////
// Init
////

void LmArts::Init()
{
  // set all skills to minimum
  for (int i = 0; i < NUM_ARTS; ++i) {
    arts_.skill[i] = Stats::SKILL_MIN;
  }
}

////
// ConvertToHost
////

void LmArts::ConvertToHost()
{
  // empty -- all chars
}

////
// ConvertToNetwork
////

void LmArts::ConvertToNetwork()
{
  // empty -- all chars
}


bool LmArts::ArtNumOK(int art_num)
{
  return ((art_num >= 0) && (art_num < NUM_ARTS));
}

bool LmArts::SkillValueOK(int skill_val)
{
  return ((skill_val >= Stats::SKILL_MIN) && (skill_val <= Stats::SKILL_MAX));
}

bool LmArts::CanSetSkill(int art_num, int skill_val) const
{
  return ArtNumOK(art_num) && SkillValueOK(skill_val);
}

int LmArts::Skill(int art_num) const
{
  return ArtNumOK(art_num) ? arts_.skill[art_num] : Stats::SKILL_MIN;
}

void LmArts::SetSkill(int art_num, int skill_val)
{
  if (CanSetSkill(art_num, skill_val)) {
    arts_.skill[art_num] = skill_val;
  }
}

////
// Dump
////

#ifdef USE_DEBUG
void LmArts::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmArts[%p,%d]: >\n"), this, sizeof(LmArts));
  for (int i = 0; i < NUM_ARTS; ++i) {
    if ((i % 10) == 0) {
      INDENT(indent + 1, f);
    }
   _ftprintf(f, _T("%02d: %02d"), i, Skill(i));
    if ((i % 10) != 9) {
     _ftprintf(f, _T(",  "));
    }
    else if (i != (NUM_ARTS - 1)) {
     _ftprintf(f, _T("\n"));
    }
  }
 _ftprintf(f, _T("\n"));
}
#endif /* USE_DEBUG */
