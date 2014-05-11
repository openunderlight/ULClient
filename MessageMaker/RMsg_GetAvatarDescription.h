// RMsg_GetAvatarDescription.h  -*- C++ -*-
// $Id: RMsg_GetAvatarDescription.h,v 1.1 1998-04-17 16:27:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// request avatar description

#ifndef INCLUDED_RMsg_GetAvatarDescription
#define INCLUDED_RMsg_GetAvatarDescription

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"

// message class

class RMsg_GetAvatarDescription : public LmMesg {

public:

  RMsg_GetAvatarDescription();
  ~RMsg_GetAvatarDescription();

  void Init(lyra_id_t playerid);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_GetAvatarDescription.i"
#endif

#endif /* INCLUDED_RMsg_GetAvatarDescription */
