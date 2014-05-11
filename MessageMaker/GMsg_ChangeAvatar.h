// GMsg_ChangeAvatar.h  -*- C++ -*-
// $Id: GMsg_ChangeAvatar.h,v 1.4 1997-07-28 21:59:02-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// change player avatar message

#ifndef INCLUDED_GMsg_ChangeAvatar
#define INCLUDED_GMsg_ChangeAvatar

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"
#include "LmAvatar.h"

// forward references

// message class

class GMsg_ChangeAvatar : public LmMesg {

public:

  enum {
    // which avatar to change
    AVATAR_PERMANENT = 'P',
    AVATAR_CURRENT   = 'C'
  };

public:

  GMsg_ChangeAvatar();
  ~GMsg_ChangeAvatar();

  void Init(const LmAvatar& avatar, int which);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  const LmAvatar& Avatar() const;
  int Which() const;

  //mutators
  void SetAvatar(const LmAvatar& avatar);
  void SetWhich(int which);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    LmAvatar avatar;
    int which;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_ChangeAvatar.i"
#endif

#endif /* INCLUDED_GMsg_ChangeAvatar */
