// GMsg_TakeItem.h  -*- C++ -*-
// $Id: GMsg_TakeItem.h,v 1.1 1997-09-18 16:51:12-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// message class template

#ifndef INCLUDED_GMsg_TakeItem
#define INCLUDED_GMsg_TakeItem

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

class GMsg_TakeItem : public LmMesg {

public:

  GMsg_TakeItem();
  ~GMsg_TakeItem();

  void Init(lyra_id_t sourceid, const LmItem& item);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t SourceID() const;
  const LmItem& Item() const;

  // mutators
  void SetSourceID(lyra_id_t sourceid);
  void SetItem(const LmItem& item);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t sourceid;
    LmItem item;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_TakeItem.i"
#endif

#endif /* INCLUDED_GMsg_TakeItem */
