// GMsg_ItemDescription.h  -*- C++ -*-
// $Id: GMsg_ItemDescription.h,v 1.1 1997-12-05 14:44:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// item description

#ifndef INCLUDED_GMsg_ItemDescription
#define INCLUDED_GMsg_ItemDescription

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// message class

class GMsg_ItemDescription : public LmMesg {

public:

  GMsg_ItemDescription();
  ~GMsg_ItemDescription();

  void Init(int serial, const TCHAR* creator, const TCHAR* description, const TCHAR* target);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Serial() const;
  const TCHAR* Creator() const;
  const TCHAR* Description() const;
  const TCHAR* Target() const;

  // mutators
  void SetSerial(int serial);
  void SetCreator(const TCHAR* creator);
  void SetDescription(const TCHAR* description);
  void SetTarget(const TCHAR* target);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  void calc_size();

  // message data structure
  struct data_t {
    int serial;
    TCHAR creator[Lyra::PLAYERNAME_MAX];
    TCHAR target[Lyra::PLAYERNAME_MAX];
    TCHAR description[Lyra::MAX_ITEMDESC];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_ItemDescription.i"
#endif

#endif /* INCLUDED_GMsg_ItemDescription */
