// GMsg_SetAvatarDescription.h  -*- C++ -*-
// $Id: GMsg_SetAvatarDescription.h,v 1.1 1998-04-17 16:25:53-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// change avatar description

#ifndef INCLUDED_GMsg_SetAvatarDescription
#define INCLUDED_GMsg_SetAvatarDescription

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// message class

class GMsg_SetAvatarDescription : public LmMesg {

public:

  GMsg_SetAvatarDescription();
  ~GMsg_SetAvatarDescription();

  void Init(const TCHAR* description);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  const TCHAR* Description() const;

  // mutators
  void SetDescription(const TCHAR* description);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual message size
  void calc_size();

  // message data structure
  struct data_t {
    TCHAR description[Lyra::MAX_AVATARDESC];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_SetAvatarDescription.i"
#endif

#endif /* INCLUDED_GMsg_SetAvatarDescription */
