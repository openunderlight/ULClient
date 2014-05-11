// RMsg_ItemDrop.h  -*- C++ -*-
// $Id: RMsg_ItemDrop.h,v 1.18 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// item (with state) drop message

#ifndef INCLUDED_RMsg_ItemDrop
#define INCLUDED_RMsg_ItemDrop

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"
#include "LmItem.h"
#include "LmPosition.h"

// forward references

// message class

class RMsg_ItemDrop : public LmMesg {

public:

  enum {
    // constants
    MAX_ITEMS = Lyra::MAX_ROOMITEMS,

    // default values
    DEFAULT_NUMITEMS = 0
  };

public:

  RMsg_ItemDrop();
  ~RMsg_ItemDrop();

  void Init(int num_items);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int NumItems() const;
  const LmItem& Item(int num) const;
  const LmPosition& Position(int num) const;

  // mutators
  void SetNumItems(int num);
  void SetItem(int num, const LmItem& item, const LmPosition& pos);

public: // logically private

  // information on an item
  struct full_item_t {
    LmItem item;
    LmPosition pos;
  };

private:

  // standard non-public methods
  void hton();
  void ntoh();

  void calc_size();
  void calc_items();

  int num_items_;  // number of items in data_.items

  // message data structure
  struct data_t {
    full_item_t items[MAX_ITEMS];
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_ItemDrop.i"
#endif

#endif /* INCLUDED_RMsg_ItemDrop */
