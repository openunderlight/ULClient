// GMsg_UpdateItem.h  -*- C++ -*-
// $Id: GMsg_UpdateItem.h,v 1.7 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// update inventory item

#ifndef INCLUDED_GMsg_UpdateItem
#define INCLUDED_GMsg_UpdateItem

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

class GMsg_UpdateItem : public LmMesg {

public:

  GMsg_UpdateItem();
  ~GMsg_UpdateItem();

  void Init(const LmItem& item);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors/mutators
  const LmItem& Item() const;
  void SetItem(const LmItem& item);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    LmItem item;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_UpdateItem.i"
#endif

#endif /* INCLUDED_GMsg_UpdateItem */
