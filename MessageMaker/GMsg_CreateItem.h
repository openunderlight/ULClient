// GMsg_CreateItem.h  -*- C++ -*-
// $Id: GMsg_CreateItem.h,v 1.5 1997-12-05 14:44:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// create item in inventory

#ifndef INCLUDED_GMsg_CreateItem
#define INCLUDED_GMsg_CreateItem

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"
#include "LmItem.h"

// forward references

// message class

class GMsg_CreateItem : public LmMesg {

public:

  GMsg_CreateItem();
  ~GMsg_CreateItem();

  void Init(const LmItem& item, const TCHAR* description = _T("none"));

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  const LmItem& Item() const;
  const TCHAR* Description() const;

  // mutators
  void SetItem(const LmItem& item);
  void SetDescription(const TCHAR* description);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  void calc_size();

  // message data structure
  struct data_t {
    LmItem item;
    TCHAR desc[Lyra::MAX_ITEMDESC];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_CreateItem.i"
#endif

#endif /* INCLUDED_GMsg_CreateItem */
