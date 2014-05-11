// RMsg_RcvAvatarDescription.h  -*- C++ -*-
// $Id: RMsg_RcvAvatarDescription.h,v 1.1 1998-04-17 16:27:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// receive avatar description from server

#ifndef INCLUDED_RMsg_RcvAvatarDescription
#define INCLUDED_RMsg_RcvAvatarDescription

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"

// message class

class RMsg_RcvAvatarDescription : public LmMesg {

public:

  RMsg_RcvAvatarDescription();
  ~RMsg_RcvAvatarDescription();

  void Init(lyra_id_t playerid, const TCHAR* description);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  const TCHAR* Description() const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetDescription(const TCHAR* description);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual message size
  void calc_size();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
    TCHAR description[Lyra::MAX_AVATARDESC];
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_RcvAvatarDescription.i"
#endif

#endif /* INCLUDED_RMsg_RcvAvatarDescription */
