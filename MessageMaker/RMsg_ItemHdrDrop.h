// RMsg_ItemHdrDrop.h  -*- C++ -*-
// $Id: RMsg_ItemHdrDrop.h,v 1.10 1997-07-29 19:32:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// item drop message -- header information only

#ifndef INCLUDED_RMsg_ItemHdrDrop
#define INCLUDED_RMsg_ItemHdrDrop

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"
#include "LmItem.h"
#include "LmItemHdr.h"
#include "LmPosition.h"

// forward references

// message class

class RMsg_ItemHdrDrop : public LmMesg {

public:

  enum {
    // constants
    MAX_ITEMS = Lyra::MAX_ROOMITEMS,

    // default values
    DEFAULT_NUMITEMS = 0
  };

public:

  RMsg_ItemHdrDrop();
  ~RMsg_ItemHdrDrop();

  void Init(int num_items);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int NumItems() const;
  const LmItemHdr& ItemHeader(int num) const;
  const LmPosition& Position(int num) const;

  // mutators
  void SetNumItems(int num);
  void SetItemHeader(int num, const LmItemHdr& itemhdr, const LmPosition& pos);
  void SetItem(int num, const LmItem& item, const LmPosition& pos);

public: // logically private

  // information on an item
  struct header_item_t {
    LmItemHdr itemhdr;
    LmPosition pos;
  };

private:

  // standard non-public methods
  void hton();
  void ntoh();

  void calc_size();
  void calc_items();

  int num_items_;  // number of items in data_.itemhdrs

  // message data structure
  struct data_t {
    header_item_t itemhdrs[MAX_ITEMS];
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_ItemHdrDrop.i"
#endif

#endif /* INCLUDED_RMsg_ItemHdrDrop */
