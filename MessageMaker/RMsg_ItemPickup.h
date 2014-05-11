// RMsg_ItemPickup.h  -*- C++ -*-
// $Id: RMsg_ItemPickup.h,v 1.17 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// itempickup message

#ifndef INCLUDED_RMsg_ItemPickup
#define INCLUDED_RMsg_ItemPickup

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "LmItem.h"
#include "LmItemHdr.h"

// forward references

// message class

class RMsg_ItemPickup : public LmMesg {

public:

  enum {
    // constants
    MAX_ITEMS = Lyra::MAX_ROOMITEMS,

    // default values
    DEFAULT_NUMITEMS = 0
  };

public:

  RMsg_ItemPickup();
  ~RMsg_ItemPickup();

  void Init(int num_items);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int NumItems() const;
  const LmItemHdr& ItemHeader(int num) const;

  // mutators
  void SetNumItems(int num_items);
  void SetItemHeader(int num, const LmItemHdr& itemhdr);
  void SetItem(int num, const LmItem& item);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  void calc_size();
  void calc_items();

  int num_items_;  // number of items in data_.itemhdrs

  // message data structure
  struct data_t {
    LmItemHdr itemhdrs[MAX_ITEMS];
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_ItemPickup.i"
#endif

#endif /* INCLUDED_RMsg_ItemPickup */
