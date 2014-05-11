// LmArts.h  -*- C++ -*-
// $Id: LmArts.h,v 1.4 1998-04-17 16:38:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// player skills

#ifndef INCLUDED_LmArts
#define INCLUDED_LmArts

#include <stdio.h>

#include "LyraDefs.h"
#include "SharedConstants.h"

// base datatype

struct lyra_arts_t {
  char skill[300]; // must be multiple of 4, > NUM_ARTS
};

// the class

class LmArts {

public:

  LmArts();

  void Init();

  // selectors/mutators
  int Skill(int art_num) const;
  void SetSkill(int art_num, int skill_val);

  bool CanSetSkill(int art_num, int skill_val) const;

  // validation
  static bool ArtNumOK(int art_num);
  static bool SkillValueOK(int skill_val);

  // byteorder conversion
  void ConvertToHost();
  void ConvertToNetwork();

  void Dump(FILE* f, int indent = 0) const;

private:

  lyra_arts_t arts_;

};

#ifdef USE_INLINE
#include "LmArts.i"
#endif

#endif /* INCLUDED_LmArts */
