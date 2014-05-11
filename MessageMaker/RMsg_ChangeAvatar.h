// RMsg_ChangeAvatar.h  -*- C++ -*-
// $Id: RMsg_ChangeAvatar.h,v 1.3 1997-07-28 21:59:12-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// change player avatar message

#ifndef INCLUDED_RMsg_ChangeAvatar
#define INCLUDED_RMsg_ChangeAvatar

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"
#include "LmAvatar.h"

// forward references

// message class

class RMsg_ChangeAvatar : public LmMesg {

public:

  RMsg_ChangeAvatar();
  ~RMsg_ChangeAvatar();

  void Init(lyra_id_t playerid, const LmAvatar& avatar);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  const LmAvatar& Avatar() const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetAvatar(const LmAvatar& avatar);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
    LmAvatar avatar;
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_ChangeAvatar.i"
#endif

#endif /* INCLUDED_RMsg_ChangeAvatar */
